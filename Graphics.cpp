#include "Graphics.h"
#include "ThrowMacros.h"
#include <d3dcommon.h>
#include <numbers>
#include <d3dcompiler.h>
#include "Utils.h"
#include "PointLight.h"
#include "Viewport.h"
#include "ScissorRectangle.h"
#include "RootSignature.h"
#include "RootParametersDescription.h"

namespace Dx = DirectX;
namespace Wrl = Microsoft::WRL;

Graphics::Graphics(const HWND& hWnd, const float windowWidth, const float windowHeight) :
	windowWidth(windowWidth),
	windowHeight(windowHeight)
{
	LoadPipeline(hWnd);
	LoadAssets();

	cbvSrvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	CreateRootSignature();

	gui = std::make_unique<Gui>(hWnd, device.Get(), bufferCount, renderTargetDxgiFormat);
}

void Graphics::RenderBegin()
{
	gui->BeginFrame();
	ResetCommandListAndAllocator();
}

void Graphics::RenderEnd()
{
	gui->EndFrame(commandList.Get());

	// select current buffer to render to
	auto& backBuffer = renderTargets[curBufferIndex];

	const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(backBuffer.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	commandList->ResourceBarrier(1, &barrier);

	CHECK_HR(commandList->Close());

	ID3D12CommandList* const commandLists[] = { commandList.Get() };
	commandQueue->ExecuteCommandLists((UINT)std::size(commandLists), commandLists);

	CHECK_HR(swapChain->Present(1, 0));

	const UINT64 currentFenceValue = fenceValues[curBufferIndex];
	CHECK_HR(commandQueue->Signal(fence.Get(), currentFenceValue));

	curBufferIndex = swapChain->GetCurrentBackBufferIndex();

	if (fence->GetCompletedValue() < fenceValues[curBufferIndex])
	{
		CHECK_HR(fence->SetEventOnCompletion(fenceValues[curBufferIndex], fenceEvent));
		WaitForSingleObjectEx(fenceEvent, INFINITE, FALSE);
	}

	// Set the fence value for the next frame.
	fenceValues[curBufferIndex] = currentFenceValue + 1;
}

void Graphics::OnDestroy()
{
	CHECK_HR(commandQueue->Signal(fence.Get(), ++fenceValues[curBufferIndex]));
	CHECK_HR(fence->SetEventOnCompletion(fenceValues[curBufferIndex], fenceEvent));
	if (WaitForSingleObject(fenceEvent, 2000) == WAIT_FAILED)
	{
		CHECK_HR(GetLastError());
	}

	CloseHandle(fenceEvent);
}

void Graphics::ExecuteBundle(ID3D12GraphicsCommandList* const bundle)
{
	commandList->ExecuteBundle(bundle);
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
			//debugController->SetEnableGPUBasedValidation(true);
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
			.Format = renderTargetDxgiFormat,
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

		for (UINT i = 0; i < bufferCount; i++)
		{
			CHECK_HR(swapChain->GetBuffer(i, IID_PPV_ARGS(&renderTargets[i])));
			device->CreateRenderTargetView(renderTargets[i].Get(), nullptr, rtvHandle);
			rtvHandle.Offset(rtvDescriptorSize);

			CHECK_HR(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocators[i])));
		}
	}

	CHECK_HR(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_BUNDLE, IID_PPV_ARGS(&bundleAllocator)));
}

void Graphics::LoadAssets()
{
	// Create the command list.
	CHECK_HR(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocators[curBufferIndex].Get(), nullptr, IID_PPV_ARGS(&commandList)));
	CHECK_HR(commandList->Close());

	CHECK_HR(device->CreateFence(fenceValues[curBufferIndex], D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));

	// fence signalling event
	fenceEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
	if (!fenceEvent)
	{
		CHECK_HR(GetLastError());
		throw std::runtime_error{ "Failed to create fence event" };
	}
}

void Graphics::CreateRootSignature()
{
	std::vector<CD3DX12_ROOT_PARAMETER> rootParameters;
	rootParameters.resize(RPD::paramsNum);

	for (const auto& cbv : RPD::cbvs)
	{
		rootParameters[cbv.ParamIndex].InitAsConstantBufferView(cbv.slot, 0u, cbv.visibility);
	}

	for (UINT i = 0; i < RPD::texturesNum; i++)
	{
		texesDescRanges.push_back(D3D12_DESCRIPTOR_RANGE{ D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1u, i, 0u, i });
	}
	rootParameters[RPD::ParamsIndexes::TexturesDescTable].InitAsDescriptorTable((UINT)texesDescRanges.size(), texesDescRanges.data(), D3D12_SHADER_VISIBILITY_PIXEL);
	rootSignature = std::make_unique<RootSignature>(*this, rootParameters);
}

void Graphics::ClearRenderTargetView()
{
	const auto& rtv = GetRtvCpuHandle();
	// transition buffer resource to render target state
	const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(renderTargets[curBufferIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	commandList->ResourceBarrier(1, &barrier);
	// calculate clear color
	const FLOAT clearColor[] = { 0.13f, 0.05f, 0.05f, 1.0f };
	// clear rtv
	commandList->ClearRenderTargetView(rtv, clearColor, 0, nullptr);
}

CD3DX12_CPU_DESCRIPTOR_HANDLE Graphics::GetRtvCpuHandle() noexcept
{
	return CD3DX12_CPU_DESCRIPTOR_HANDLE
	{
		rtvHeap->GetCPUDescriptorHandleForHeapStart(),
		(INT)curBufferIndex, rtvDescriptorSize
	};
}

ID3D12GraphicsCommandList* Graphics::GetMainCommandList()
{
	return commandList.Get();
}

Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> Graphics::CreateBundle()
{
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> bundle;
	CHECK_HR(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_BUNDLE, bundleAllocator.Get(), nullptr, IID_PPV_ARGS(&bundle)));
	return bundle;
}

ID3D12Device2* Graphics::GetDevice()
{
	return device.Get();
}

PipelineState::PipelineStateStream Graphics::GetCommonPSS()
{
	PipelineState::PipelineStateStream commonPipelineStateStream;
	commonPipelineStateStream.primitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	commonPipelineStateStream.renderTargetFormats = {
		.RTFormats{ renderTargetDxgiFormat },
		.NumRenderTargets = 1,
	};
	commonPipelineStateStream.dsvFormat = DXGI_FORMAT_D32_FLOAT;
	auto dsDesc = CD3DX12_DEPTH_STENCIL_DESC(CD3DX12_DEFAULT{});
	//dsDesc.DepthFunc = D3D12_COMPARISON_FUNC_GREATER;
	commonPipelineStateStream.depthStencil = dsDesc;
	return commonPipelineStateStream;
}

RootSignature* Graphics::GetRootSignature()
{
	return rootSignature.get();
}

UINT Graphics::GetCbvSrvDescriptorSize() const noexcept
{
	return cbvSrvDescriptorSize;
}

//void Graphics::OffsetCbvSrvCpuHandle(INT descNum)
//{
//	srvCpuHandle.Offset(descNum, cbvSrvDescriptorSize);
//}

float Graphics::GetWindowWidth() const noexcept
{
	return windowWidth;
}

float Graphics::GetWindowHeight() const noexcept
{
	return windowHeight;
}

std::vector<CD3DX12_ROOT_PARAMETER>& Graphics::GetCommonRootParametersRef() noexcept
{
	return commonRootParameters;
}

void Graphics::SetProjection(const DirectX::XMMATRIX proj) noexcept
{
	DirectX::XMStoreFloat4x4(&projection, proj);
}

void Graphics::SetProjection(const DirectX::XMFLOAT4X4 proj) noexcept
{
	projection = proj;
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
	CHECK_HR(commandAllocators[curBufferIndex]->Reset());
	CHECK_HR(commandList->Reset(commandAllocators[curBufferIndex].Get(), nullptr));
}

void Graphics::ExecuteCommandList()
{
	// submit command list to queue as array with single element
	ID3D12CommandList* const commandLists[] = { commandList.Get() };
	commandQueue->ExecuteCommandLists((UINT)std::size(commandLists), commandLists);
}

void Graphics::WaitForQueueFinish()
{
	CHECK_HR(commandQueue->Signal(fence.Get(), ++fenceValues[curBufferIndex]));
	CHECK_HR(fence->SetEventOnCompletion(fenceValues[curBufferIndex], fenceEvent));
	if (WaitForSingleObject(fenceEvent, INFINITE) == WAIT_FAILED)
	{
		CHECK_HR(GetLastError());
	}
}

void Graphics::Signal()
{
	CHECK_HR(commandQueue->Signal(fence.Get(), ++fenceValues[curBufferIndex]));
}

void Graphics::WaitForSignal()
{
	CHECK_HR(fence->SetEventOnCompletion(fenceValues[curBufferIndex], fenceEvent));
	if (WaitForSingleObject(fenceEvent, INFINITE) == WAIT_FAILED)
	{
		CHECK_HR(GetLastError());
	}
}

Gui* const Graphics::GetGui() noexcept
{
	return gui.get();
}