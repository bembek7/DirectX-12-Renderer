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

//static void LogToDebugOutput(const std::string& message)
//{
//	OutputDebugStringA(message.c_str());
//}
//
//void AnalyzeBreadcrumbs(const D3D12_DRED_AUTO_BREADCRUMBS_OUTPUT& breadcrumbsOutput)
//{
//	const D3D12_AUTO_BREADCRUMB_NODE* pNode = breadcrumbsOutput.pHeadAutoBreadcrumbNode;
//	while (pNode)
//	{
//		std::ostringstream logStream;
//		logStream << "Command List: " << (pNode->pCommandListDebugNameA ? pNode->pCommandListDebugNameA : "Unknown") << "\n";
//		logStream << "Command Queue: " << (pNode->pCommandQueueDebugNameA ? pNode->pCommandQueueDebugNameA : "Unknown") << "\n";
//		logStream << "Breadcrumb Contexts:\n";
//
//		for (UINT i = 0; i < pNode->BreadcrumbCount; ++i)
//		{
//			logStream << "  " << i << ": " << pNode->pCommandHistory[i] << "\n";
//		}
//
//		LogToDebugOutput(logStream.str());
//
//		pNode = pNode->pNext;
//	}
//}
//
//void AnalyzePageFault(const D3D12_DRED_PAGE_FAULT_OUTPUT& pageFaultOutput)
//{
//	std::ostringstream logStream;
//	logStream << "Page Fault Address: " << pageFaultOutput.PageFaultVA << "\n";
//
//	// Log existing allocation nodes
//	const D3D12_DRED_ALLOCATION_NODE* pNode = pageFaultOutput.pHeadExistingAllocationNode;
//	logStream << "Existing Allocations:\n";
//	while (pNode)
//	{
//		logStream << "  Allocation Type: " << pNode->AllocationType << "\n";
//		logStream << "  ObjectNameA: " << pNode->ObjectNameA << "\n";
//		pNode = pNode->pNext;
//	}
//
//	// Log recent freed allocation nodes
//	pNode = pageFaultOutput.pHeadRecentFreedAllocationNode;
//	logStream << "Recent Freed Allocations:\n";
//	while (pNode)
//	{
//		logStream << "  Allocation Type: " << pNode->AllocationType << "\n";
//		logStream << "  ObjectNameA: " << pNode->ObjectNameA << "\n";
//		pNode = pNode->pNext;
//	}
//
//	LogToDebugOutput(logStream.str());
//}

Graphics::Graphics(const HWND& hWnd, const float windowWidth, const float windowHeight) :
	windowWidth(windowWidth),
	windowHeight(windowHeight)
{
	LoadPipeline(hWnd);
	LoadAssets();

	cbvSrvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	gui = std::make_unique<Gui>(hWnd, device.Get(), bufferCount, renderTargetDxgiFormat);
}

void Graphics::RenderBegin()
{
	gui->BeginFrame();
	ResetCommandListAndAllocator();
}

void Graphics::RenderEnd()
{
	gui->EndFrame(GetRtvCpuHandle(), commandList.Get());

	// select current buffer to render to
	auto& backBuffer = renderTargets[curBufferIndex];

	const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(backBuffer.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	commandList->ResourceBarrier(1, &barrier);

	CHECK_HR(commandList->Close());

	ID3D12CommandList* const commandLists[] = { commandList.Get() };
	commandQueue->ExecuteCommandLists((UINT)std::size(commandLists), commandLists);

	if (swapChain->Present(1, 0) < 0)
	{
		CHECK_HR(device->GetDeviceRemovedReason());

		/*Wrl::ComPtr<ID3D12DeviceRemovedExtendedData> pDred;
		CHECK_HR(device->QueryInterface(IID_PPV_ARGS(&pDred)));
		D3D12_DRED_AUTO_BREADCRUMBS_OUTPUT DredAutoBreadcrumbsOutput;
		D3D12_DRED_PAGE_FAULT_OUTPUT DredPageFaultOutput;
		CHECK_HR(pDred->GetAutoBreadcrumbsOutput(&DredAutoBreadcrumbsOutput));
		CHECK_HR(pDred->GetPageFaultAllocationOutput(&DredPageFaultOutput));
		AnalyzeBreadcrumbs(DredAutoBreadcrumbsOutput);
		AnalyzePageFault(DredPageFaultOutput);*/
	}

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
		CHECK_HR(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
		
		debugController->EnableDebugLayer();
		//debugController->SetEnableGPUBasedValidation(true);

		/*Wrl::ComPtr<ID3D12DeviceRemovedExtendedDataSettings> pDredSettings; not supported on my device
		CHECK_HR(D3D12GetDebugInterface(IID_PPV_ARGS(&pDredSettings)));

		pDredSettings->SetAutoBreadcrumbsEnablement(D3D12_DRED_ENABLEMENT_FORCED_ON);
		pDredSettings->SetPageFaultEnablement(D3D12_DRED_ENABLEMENT_FORCED_ON);*/
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

void Graphics::ClearRenderTargetView()
{
	const auto& rtv = GetRtvCpuHandle();
	// transition buffer resource to render target state
	const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(renderTargets[curBufferIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	commandList->ResourceBarrier(1, &barrier);
	// clear rtv
	commandList->ClearRenderTargetView(rtv, Graphics::clearColor4.data(), 0, nullptr);
}

CD3DX12_CPU_DESCRIPTOR_HANDLE Graphics::GetRtvCpuHandle() const noexcept
{
	return CD3DX12_CPU_DESCRIPTOR_HANDLE
	{
		rtvHeap->GetCPUDescriptorHandleForHeapStart(),
		(INT)curBufferIndex, rtvDescriptorSize
	};
}

CD3DX12_CPU_DESCRIPTOR_HANDLE* Graphics::GetDSVHandle() noexcept
{
	return &dethPrePassDSVHandle;
}

void Graphics::SetDSVHandle(const CD3DX12_CPU_DESCRIPTOR_HANDLE& dsvHandle) noexcept
{
	dethPrePassDSVHandle = dsvHandle;
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

UINT Graphics::GetCbvSrvDescriptorSize() const noexcept
{
	return cbvSrvDescriptorSize;
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