#include "Graphics.h"
#include "ThrowMacros.h"
#include <d3dcommon.h>
#include <numbers>
#include <d3dcompiler.h>
#include "Utils.h"

#include "Viewport.h"
#include "ScissorRectangle.h"

namespace Dx = DirectX;
namespace Wrl = Microsoft::WRL;

Graphics::Graphics(const HWND& hWnd, const float windowWidth, const float windowHeight) :
	windowWidth(windowWidth),
	windowHeight(windowHeight)
{
	{
		// setup perspective projection matrix
		const auto aspectRatio = windowWidth / windowHeight;
		const auto projection = Dx::XMMatrixPerspectiveFovLH(Dx::XMConvertToRadians(65.f), aspectRatio, 0.1f, 100.0f);

		SetProjection(projection);
	}

	LoadPipeline(hWnd);
	LoadAssets();
}

void Graphics::OnUpdate(const float time)
{
	mesh->OnUpdate(time);
}

void Graphics::OnRender()
{
	// Record all the commands we need to render the scene into the command list.
	PopulateCommandList();

	// Execute the command list.
	ID3D12CommandList* const commandLists[] = { commandList.Get() };
	commandQueue->ExecuteCommandLists((UINT)std::size(commandLists), commandLists);

	// Present the frame.
	CHECK_HR(swapChain->Present(1, 0));

	// WAITING FOR THE FRAME TO COMPLETE BEFORE CONTINUING IS NOT BEST PRACTICE.
	// This is code implemented as such for simplicity. More advanced samples
	// illustrate how to use fences for efficient resource usage.
	fence->WaitForQueueFinish(*this, INFINITE);
}

void Graphics::OnDestroy()
{
	fence->WaitForQueueFinish(*this, 2000);

	fence->CloseEventHandle();
}

void Graphics::DrawIndexed(const UINT indicesNumber)
{
	commandList->DrawIndexedInstanced(indicesNumber, 1, 0, 0, 0);
}

void Graphics::LoadPipeline(const HWND& hWnd)
{
	// Enable the D3D12 debug layer.
	{
#if defined(_DEBUG)
		Wrl::ComPtr<ID3D12Debug1> debugController;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
		{
			debugController->EnableDebugLayer();
			debugController->SetEnableGPUBasedValidation(true);
		}
#endif
	}

	// dxgi factory
	Wrl::ComPtr<IDXGIFactory4> dxgiFactory;
	CHECK_HR(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&dxgiFactory)));

	// device
	CHECK_HR(D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&device)));

	// command queue
	{
		const D3D12_COMMAND_QUEUE_DESC desc = {
			.Type = D3D12_COMMAND_LIST_TYPE_DIRECT,
			.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,
			.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE,
			.NodeMask = 0,
		};
		CHECK_HR(device->CreateCommandQueue(&desc, IID_PPV_ARGS(&commandQueue)));
	}

	// swap chain
	{
		const DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {
			.Width = (UINT)windowWidth,
			.Height = (UINT)windowHeight,
			.Format = DXGI_FORMAT_R8G8B8A8_UNORM,
			.Stereo = FALSE,
			.SampleDesc = {
				.Count = 1,
				.Quality = 0
			},
			.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
			.BufferCount = bufferCount,
			.Scaling = DXGI_SCALING_STRETCH,
			.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
			.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED,
			.Flags = 0,
		};
		Wrl::ComPtr<IDXGISwapChain1> swapChain1;
		CHECK_HR(dxgiFactory->CreateSwapChainForHwnd(
			commandQueue.Get(),
			hWnd,
			&swapChainDesc,
			nullptr,
			nullptr,
			&swapChain1));
		CHECK_HR(swapChain1.As(&swapChain));
	}

	// rtv descriptor heap
	{
		const D3D12_DESCRIPTOR_HEAP_DESC desc = {
			.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
			.NumDescriptors = bufferCount,
		};
		CHECK_HR(device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&rtvHeap)));
	}
	rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	// Create frame resources.
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart());
		for (int i = 0; i < bufferCount; i++)
		{
			CHECK_HR(swapChain->GetBuffer(i, IID_PPV_ARGS(&renderTargets[i])));
			device->CreateRenderTargetView(renderTargets[i].Get(), nullptr, rtvHandle);
			rtvHandle.Offset(rtvDescriptorSize);
		}
	}

	CHECK_HR(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator)));
}

void Graphics::LoadAssets()
{
	// Create the command list.
	CHECK_HR(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator.Get(), nullptr, IID_PPV_ARGS(&commandList)));

	// Command lists are created in the recording state, but there is nothing
	// to record yet. The main loop expects it to be closed, so close it now.
	CHECK_HR(commandList->Close());

	fence = std::make_unique<Fence>(*this);

	mesh = std::make_unique<Mesh>(*this);

	// define scissor rect
	scissorRect = std::make_unique<ScissorRectangle>();
	// define viewport
	viewport = std::make_unique<Viewport>(windowWidth, windowHeight);
}

void Graphics::PopulateCommandList()
{
	curBackBufferIndex = swapChain->GetCurrentBackBufferIndex();
	// select current buffer to render to
	auto& backBuffer = renderTargets[curBackBufferIndex];

	// reset command list and allocator
	ResetCommandListAndAllocator();

	// get rtv handle for the buffer used in this frame
	const CD3DX12_CPU_DESCRIPTOR_HANDLE rtv
	{
		rtvHeap->GetCPUDescriptorHandleForHeapStart(),
		(INT)curBackBufferIndex, rtvDescriptorSize
	};

	ClearRenderTarget(backBuffer.Get(), rtv);

	// configure IA
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	/*vertexBuffer->Bind(*this);
	indexBuffer->Bind(*this);*/
	mesh->Bind(*this);

	// configure RS
	viewport->Bind(*this);
	scissorRect->Bind(*this);
	// bind render target
	commandList->OMSetRenderTargets(1, &rtv, TRUE, nullptr);

	// draw the geometry
	commandList->DrawIndexedInstanced(mesh->GetIndicesNumber(), 1, 0, 0, 0);
	// prepare buffer for presentation by transitioning to present state
	{
		const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(backBuffer.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		commandList->ResourceBarrier(1, &barrier);
	}

	CHECK_HR(commandList->Close());
}

void Graphics::ClearRenderTarget(ID3D12Resource* const backBuffer, const CD3DX12_CPU_DESCRIPTOR_HANDLE& rtv)
{
	// transition buffer resource to render target state
	const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(backBuffer, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	commandList->ResourceBarrier(1, &barrier);
	// calculate clear color
	const FLOAT clearColor[] = { 0.13f, 0.05f, 0.05f, 1.0f };
	// clear rtv
	commandList->ClearRenderTargetView(rtv, clearColor, 0, nullptr);
}

float Graphics::GetWindowWidth() const noexcept
{
	return windowWidth;
}

float Graphics::GetWindowHeight() const noexcept
{
	return windowHeight;
}

void Graphics::SetProjection(const DirectX::XMMATRIX proj) noexcept
{
	DirectX::XMStoreFloat4x4(&projection, proj);
}

DirectX::XMMATRIX Graphics::GetProjection() const noexcept
{
	return DirectX::XMLoadFloat4x4(&projection);
}

void Graphics::SetCamera(const DirectX::XMMATRIX cam) noexcept
{
	DirectX::XMStoreFloat4x4(&camera, cam);
}

DirectX::XMMATRIX Graphics::GetCamera() const noexcept
{
	return DirectX::XMLoadFloat4x4(&camera);
}

void Graphics::ResetCommandListAndAllocator()
{
	// reset command list and allocator
	CHECK_HR(commandAllocator->Reset());
	CHECK_HR(commandList->Reset(commandAllocator.Get(), nullptr));
}

void Graphics::ExecuteCommandList()
{
	// submit command list to queue as array with single element
	ID3D12CommandList* const commandLists[] = { commandList.Get() };
	commandQueue->ExecuteCommandLists((UINT)std::size(commandLists), commandLists);
}