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
#include "DXRHelper.h"
#include "nv_helpers_dx12/BottomLevelASGenerator.h"
#include "nv_helpers_dx12/RaytracingPipelineGenerator.h"   
#include "nv_helpers_dx12/RootSignatureGenerator.h"

namespace Dx = DirectX;
namespace Wrl = Microsoft::WRL;

inline uint64_t AlignUp(uint64_t value, uint64_t alignment) {
	return (value + (alignment - 1)) & ~(alignment - 1);
}
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
	CreateAccelerationStructures();
	CreateRaytracingPipeline(); // #DXR
	CreateRaytracingOutputBuffer(); // #DXR
	CreateShaderResourceHeap(); // #DXR
	CreateShaderBindingTable();

	cbvSrvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	dsvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

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
	Microsoft::WRL::ComPtr<IDXGIAdapter> warpAdapter;
	CHECK_HR(dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter)));
	CHECK_HR(D3D12CreateDevice(warpAdapter.Get(), D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&device)));
	D3D12_FEATURE_DATA_D3D12_OPTIONS5 caps = {};
	HRESULT hr;
	hr = device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS5, &caps, sizeof(caps));
	if (FAILED(hr) || caps.RaytracingTier < D3D12_RAYTRACING_TIER_1_0)
	{
		throw std::runtime_error("Device or driver does not support ray tracing!");
	}

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
	//const auto& rtv = GetRtvCpuHandle();
	//// transition buffer resource to render target state
	//const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(renderTargets[curBufferIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	//commandList->ResourceBarrier(1, &barrier);
	//// clear rtv
	//commandList->ClearRenderTargetView(rtv, Graphics::clearColor4.data(), 0, nullptr);

	std::vector<ID3D12DescriptorHeap*> heaps = { srvUavHeap.Get() };
	commandList->SetDescriptorHeaps(static_cast<UINT>(heaps.size()),
		heaps.data());

	CD3DX12_RESOURCE_BARRIER transition = CD3DX12_RESOURCE_BARRIER::Transition(
		outputResource.Get(), D3D12_RESOURCE_STATE_COPY_SOURCE,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	commandList->ResourceBarrier(1, &transition);

	// Setup the raytracing task
	D3D12_DISPATCH_RAYS_DESC desc = {};
	// The layout of the SBT is as follows: ray generation shader, miss
	// shaders, hit groups. As described in the CreateShaderBindingTable method,
	// all SBT entries of a given type have the same size to allow a fixed stride.

	// The ray generation shaders are always at the beginning of the SBT. 
	uint32_t rayGenerationSectionSizeInBytes = sbtHelper.GetRayGenSectionSize();
	desc.RayGenerationShaderRecord.StartAddress = sbtStorage->GetGPUVirtualAddress();
	desc.RayGenerationShaderRecord.SizeInBytes = rayGenerationSectionSizeInBytes;

	// The miss shaders are in the second SBT section, right after the ray
	// generation shader. We have one miss shader for the camera rays and one
	// for the shadow rays, so this section has a size of 2*m_sbtEntrySize. We
	// also indicate the stride between the two miss shaders, which is the size
	// of a SBT entry
	uint32_t missSectionSizeInBytes = sbtHelper.GetMissSectionSize();
	desc.MissShaderTable.StartAddress =
		sbtStorage->GetGPUVirtualAddress() + rayGenerationSectionSizeInBytes;
	desc.MissShaderTable.SizeInBytes = missSectionSizeInBytes;
	desc.MissShaderTable.StrideInBytes = sbtHelper.GetMissEntrySize();

	// The hit groups section start after the miss shaders. In this sample we
	// have one 1 hit group for the triangle
	uint32_t hitGroupsSectionSize = sbtHelper.GetHitGroupSectionSize();
	desc.HitGroupTable.StartAddress = AlignUp(
		sbtStorage->GetGPUVirtualAddress() +
		rayGenerationSectionSizeInBytes +
		missSectionSizeInBytes,
		D3D12_RAYTRACING_SHADER_TABLE_BYTE_ALIGNMENT);

	desc.HitGroupTable.SizeInBytes = hitGroupsSectionSize;
	desc.HitGroupTable.StrideInBytes = AlignUp(sbtHelper.GetHitGroupEntrySize(), D3D12_RAYTRACING_SHADER_RECORD_BYTE_ALIGNMENT);

	// Dimensions of the image to render, identical to a kernel launch dimension
	desc.Width = windowWidth;
	desc.Height = windowHeight;
	desc.Depth = 1;

	// Bind the raytracing pipeline
	commandList->SetPipelineState1(stateObject.Get());
	// Dispatch the rays and write to the raytracing output
	commandList->DispatchRays(&desc);

	// The raytracing output needs to be copied to the actual render target used
// for display. For this, we need to transition the raytracing output from a
// UAV to a copy source, and the render target buffer to a copy destination.
// We can then do the actual copy, before transitioning the render target
// buffer into a render target, that will be then used to display the image
	transition = CD3DX12_RESOURCE_BARRIER::Transition(
		outputResource.Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_COPY_SOURCE);
	commandList->ResourceBarrier(1, &transition);
	transition = CD3DX12_RESOURCE_BARRIER::Transition(
		renderTargets[curBufferIndex].Get(), D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_COPY_DEST);
	commandList->ResourceBarrier(1, &transition);

	commandList->CopyResource(renderTargets[curBufferIndex].Get(),
		outputResource.Get());

	transition = CD3DX12_RESOURCE_BARRIER::Transition(
		renderTargets[curBufferIndex].Get(), D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATE_RENDER_TARGET);
	commandList->ResourceBarrier(1, &transition);

	commandList->Close();
	ExecuteCommandList();
	WaitForQueueFinish();
	ResetCommandListAndAllocator();
}

CD3DX12_CPU_DESCRIPTOR_HANDLE Graphics::GetRtvCpuHandle() const noexcept
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

UINT Graphics::GetCbvSrvDescriptorSize() const noexcept
{
	return cbvSrvDescriptorSize;
}

UINT Graphics::GetDsvDescriptorSize() const noexcept
{
	return dsvDescriptorSize;
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

void Graphics::CreateSRV(ID3D12Resource* const resource, const CD3DX12_CPU_DESCRIPTOR_HANDLE& srvCpuHandle, const D3D12_SRV_DIMENSION viewDimension)
{
	auto resourceDesc = resource->GetDesc();
	auto format = resourceDesc.Format;
	if (format == DXGI_FORMAT_D32_FLOAT)
	{
		format = DXGI_FORMAT_R32_FLOAT;
	}
	const D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {
		.Format = format,
		.ViewDimension = viewDimension,
		.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
		.Texture2D{.MipLevels = resourceDesc.MipLevels },
	};
	device->CreateShaderResourceView(resource, &srvDesc, srvCpuHandle);
}


Graphics::AccelerationStructureBuffers Graphics::CreateBottomLevelAS(const std::vector<std::pair<ID3D12Resource*, uint32_t>>& vVertexBuffers)
{
	nv_helpers_dx12::BottomLevelASGenerator bottomLevelAS;


	struct Vertex // TODO 
	{
		Dx::XMFLOAT3 position;
	};
	// Adding all vertex buffers and not transforming their position.
	for (const auto& buffer : vVertexBuffers) {
		bottomLevelAS.AddVertexBuffer(buffer.first, 0, buffer.second,
			sizeof(Vertex), 0, 0);
	}

	// The AS build requires some scratch space to store temporary information.
	// The amount of scratch memory is dependent on the scene complexity.
	UINT64 scratchSizeInBytes = 0;
	// The final AS also needs to be stored in addition to the existing vertex
	// buffers. It size is also dependent on the scene complexity.
	UINT64 resultSizeInBytes = 0;

	bottomLevelAS.ComputeASBufferSizes(device.Get(), false, &scratchSizeInBytes,
		&resultSizeInBytes);

	// Once the sizes are obtained, the application is responsible for allocating
	// the necessary buffers. Since the entire generation will be done on the GPU,
	// we can directly allocate those on the default heap
	AccelerationStructureBuffers buffers;
	buffers.pScratch = nv_helpers_dx12::CreateBuffer(
		device.Get(), scratchSizeInBytes,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COMMON,
		nv_helpers_dx12::kDefaultHeapProps);
	buffers.pResult = nv_helpers_dx12::CreateBuffer(
		device.Get(), resultSizeInBytes,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE,
		nv_helpers_dx12::kDefaultHeapProps);

	// Build the acceleration structure. Note that this call integrates a barrier
	// on the generated AS, so that it can be used to compute a top-level AS right
	// after this method.
	bottomLevelAS.Generate(commandList.Get(), buffers.pScratch.Get(),
		buffers.pResult.Get(), false, nullptr);

	return buffers;
}

void Graphics::CreateTopLevelAS(const std::vector<std::pair<ID3D12Resource*, DirectX::XMMATRIX>>& instances)
{
	// Gather all the instances into the builder helper
	for (size_t i = 0; i < instances.size(); i++) {
		topLevelASGenerator.AddInstance(instances[i].first,
			instances[i].second, static_cast<UINT>(i),
			static_cast<UINT>(0));
	}

	// As for the bottom-level AS, the building the AS requires some scratch space
	// to store temporary data in addition to the actual AS. In the case of the
	// top-level AS, the instance descriptors also need to be stored in GPU
	// memory. This call outputs the memory requirements for each (scratch,
	// results, instance descriptors) so that the application can allocate the
	// corresponding memory
	UINT64 scratchSize, resultSize, instanceDescsSize;

	topLevelASGenerator.ComputeASBufferSizes(device.Get(), true, &scratchSize,
		&resultSize, &instanceDescsSize);

	// Create the scratch and result buffers. Since the build is all done on GPU,
	// those can be allocated on the default heap
	topLevelASBuffers.pScratch = nv_helpers_dx12::CreateBuffer(
		device.Get(), scratchSize, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		nv_helpers_dx12::kDefaultHeapProps);
	topLevelASBuffers.pResult = nv_helpers_dx12::CreateBuffer(
		device.Get(), resultSize, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE,
		nv_helpers_dx12::kDefaultHeapProps);

	// The buffer describing the instances: ID, shader binding information,
	// matrices ... Those will be copied into the buffer by the helper through
	// mapping, so the buffer has to be allocated on the upload heap.
	topLevelASBuffers.pInstanceDesc = nv_helpers_dx12::CreateBuffer(
		device.Get(), instanceDescsSize, D3D12_RESOURCE_FLAG_NONE,
		D3D12_RESOURCE_STATE_GENERIC_READ, nv_helpers_dx12::kUploadHeapProps);

	// After all the buffers are allocated, or if only an update is required, we
	// can build the acceleration structure. Note that in the case of the update
	// we also pass the existing AS as the 'previous' AS, so that it can be
	// refitted in place.
	topLevelASGenerator.Generate(commandList.Get(),
		topLevelASBuffers.pScratch.Get(),
		topLevelASBuffers.pResult.Get(),
		topLevelASBuffers.pInstanceDesc.Get());
}

void Graphics::CreateAccelerationStructures()
{
	// Build the bottom AS from the Triangle vertex buffer
	std::vector<float> vertices =
	{
		0.5f, 0.5f, 0.5f,
		0.5f, -0.5f, 0.5f,
		0.5f, 0.5f, -0.5f,
	};
	vertexBuffer = std::make_unique<VertexBuffer>(*this, vertices, UINT(sizeof(float) * 3), 3);
	ResetCommandListAndAllocator();

	AccelerationStructureBuffers bottomLevelBuffers = CreateBottomLevelAS({ {vertexBuffer->GetBuffer(), 3} });

	// Just one instance for now
	instances = { {bottomLevelBuffers.pResult.Get(), DirectX::XMMatrixIdentity()}};
	CreateTopLevelAS(instances);

	// Flush the command list and wait for it to finish
	commandList->Close();
	ExecuteCommandList();
	WaitForQueueFinish();

	// Store the AS buffers. The rest of the buffers will be released once we exit
	// the function
	bottomLevelAS = bottomLevelBuffers.pResult;
}

Microsoft::WRL::ComPtr<ID3D12RootSignature> Graphics::CreateRayGenSignature()
{
	nv_helpers_dx12::RootSignatureGenerator rsc;
	rsc.AddHeapRangesParameter(
		{ 
			{
				0 /*u0*/, 1 /*1 descriptor */, 0 /*use the implicit register space 0*/,
				D3D12_DESCRIPTOR_RANGE_TYPE_UAV /* UAV representing the output buffer*/,
				0 /*heap slot where the UAV is defined*/
			},
			{
				0 /*t0*/, 1, 0,
				D3D12_DESCRIPTOR_RANGE_TYPE_SRV /*Top-level acceleration structure*/,
				1
			} 
		}
	);

	return rsc.Generate(device.Get(), true);
}

Microsoft::WRL::ComPtr<ID3D12RootSignature> Graphics::CreateMissSignature()
{
	nv_helpers_dx12::RootSignatureGenerator rsc;
	return rsc.Generate(device.Get(), true);
}

Microsoft::WRL::ComPtr<ID3D12RootSignature> Graphics::CreateHitSignature()
{
	nv_helpers_dx12::RootSignatureGenerator rsc;
	return rsc.Generate(device.Get(), true);
}

void Graphics::CreateRaytracingPipeline()
{
	nv_helpers_dx12::RayTracingPipelineGenerator pipeline(device.Get());
	
	// The pipeline contains the DXIL code of all the shaders potentially executed
	// during the raytracing process. This section compiles the HLSL code into a
	// set of DXIL libraries. We chose to separate the code in several libraries
	// by semantic (ray generation, hit, miss) for clarity. Any code layout can be
	// used.
	
	rayGenLibrary = nv_helpers_dx12::CompileShaderLibrary(L"RayGen.hlsl");
	missLibrary = nv_helpers_dx12::CompileShaderLibrary(L"Miss.hlsl");
	hitLibrary = nv_helpers_dx12::CompileShaderLibrary(L"Hit.hlsl");
	
	// In a way similar to DLLs, each library is associated with a number of
	  // exported symbols. This
	  // has to be done explicitly in the lines below. Note that a single library
	  // can contain an arbitrary number of symbols, whose semantic is given in HLSL
	  // using the [shader("xxx")] syntax
	pipeline.AddLibrary(rayGenLibrary.Get(), { L"RayGen" });
	pipeline.AddLibrary(missLibrary.Get(), { L"Miss" });
	pipeline.AddLibrary(hitLibrary.Get(), { L"ClosestHit" });
	
	// To be used, each DX12 shader needs a root signature defining which
	// parameters and buffers will be accessed.
	rayGenSignature = CreateRayGenSignature();
	missSignature = CreateMissSignature();
	hitSignature = CreateHitSignature();

	// Hit group for the triangles, with a shader simply interpolating vertex
	// colors
	pipeline.AddHitGroup(L"HitGroup", L"ClosestHit");

	// The following section associates the root signature to each shader. Note
	// that we can explicitly show that some shaders share the same root signature
	// (eg. Miss and ShadowMiss). Note that the hit shaders are now only referred
	// to as hit groups, meaning that the underlying intersection, any-hit and
	// closest-hit shaders share the same root signature.
	pipeline.AddRootSignatureAssociation(rayGenSignature.Get(), { L"RayGen" });
	pipeline.AddRootSignatureAssociation(missSignature.Get(), { L"Miss" });
	pipeline.AddRootSignatureAssociation(hitSignature.Get(), { L"HitGroup" });

	// The payload size defines the maximum size of the data carried by the rays,
	// ie. the the data
	// exchanged between shaders, such as the HitInfo structure in the HLSL code.
	// It is important to keep this value as low as possible as a too high value
	// would result in unnecessary memory consumption and cache trashing.
	pipeline.SetMaxPayloadSize(4 * sizeof(float)); // RGB + distance

	// Upon hitting a surface, DXR can provide several attributes to the hit. In
	// our sample we just use the barycentric coordinates defined by the weights
	// u,v of the last two vertices of the triangle. The actual barycentrics can
	// be obtained using float3 barycentrics = float3(1.f-u-v, u, v);
	pipeline.SetMaxAttributeSize(2 * sizeof(float)); // barycentric coordinates

	// The raytracing process can shoot rays from existing hit points, resulting
	// in nested TraceRay calls. Our sample code traces only primary rays, which
	// then requires a trace depth of 1. Note that this recursion depth should be
	// kept to a minimum for best performance. Path tracing algorithms can be
	// easily flattened into a simple loop in the ray generation.
	pipeline.SetMaxRecursionDepth(1);

	// Compile the pipeline for execution on the GPU
	stateObject = pipeline.Generate();

	// Cast the state object into a properties object, allowing to later access
	// the shader pointers by name
	CHECK_HR(stateObject->QueryInterface(IID_PPV_ARGS(&stateObjectProps)));
}

void Graphics::CreateRaytracingOutputBuffer()
{
	D3D12_RESOURCE_DESC resDesc = {};
	resDesc.DepthOrArraySize = 1;
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	// The backbuffer is actually DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, but sRGB
	// formats cannot be used with UAVs. For accuracy we should convert to sRGB
	// ourselves in the shader
	resDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	resDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	resDesc.Width = windowWidth;
	resDesc.Height = windowHeight;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	CHECK_HR(device->CreateCommittedResource(
		&nv_helpers_dx12::kDefaultHeapProps, D3D12_HEAP_FLAG_NONE, &resDesc,
		D3D12_RESOURCE_STATE_COPY_SOURCE, nullptr,
		IID_PPV_ARGS(&outputResource)));
}

void Graphics::CreateShaderResourceHeap()
{
	// Create a SRV/UAV/CBV descriptor heap. We need 2 entries - 1 UAV for the
    // raytracing output and 1 SRV for the TLAS
	srvUavHeap = nv_helpers_dx12::CreateDescriptorHeap(
		device.Get(), 2, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, true);

	// Get a handle to the heap memory on the CPU side, to be able to write the
	// descriptors directly
	D3D12_CPU_DESCRIPTOR_HANDLE srvHandle =
		srvUavHeap->GetCPUDescriptorHandleForHeapStart();

	// Create the UAV. Based on the root signature we created it is the first
	// entry. The Create*View methods write the view information directly into
	// srvHandle
	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
	device->CreateUnorderedAccessView(outputResource.Get(), nullptr, &uavDesc,
		srvHandle);

	// Add the Top Level AS SRV right after the raytracing output buffer
	srvHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_RAYTRACING_ACCELERATION_STRUCTURE;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.RaytracingAccelerationStructure.Location =
		topLevelASBuffers.pResult->GetGPUVirtualAddress();
	// Write the acceleration structure view in the heap
	device->CreateShaderResourceView(nullptr, &srvDesc, srvHandle);
}

void Graphics::CreateShaderBindingTable()
{
	// The SBT helper class collects calls to Add*Program.  If called several
  // times, the helper must be emptied before re-adding shaders.
	sbtHelper.Reset();

	// The pointer to the beginning of the heap is the only parameter required by
	// shaders without root parameters
	D3D12_GPU_DESCRIPTOR_HANDLE srvUavHeapHandle =
		srvUavHeap->GetGPUDescriptorHandleForHeapStart();

	// The helper treats both root parameter pointers and heap pointers as void*,
	// while DX12 uses the
	// D3D12_GPU_DESCRIPTOR_HANDLE to define heap pointers. The pointer in this
	// struct is a UINT64, which then has to be reinterpreted as a pointer.
	auto heapPointer = reinterpret_cast<void*>(srvUavHeapHandle.ptr);

	// The ray generation only uses heap data
	sbtHelper.AddRayGenerationProgram(L"RayGen", { heapPointer });

	// The miss and hit shaders do not access any external resources: instead they
	// communicate their results through the ray payload
	sbtHelper.AddMissProgram(L"Miss", {});

	// Adding the triangle hit shader
	sbtHelper.AddHitGroup(L"HitGroup", {});

	// Compute the size of the SBT given the number of shaders and their
	// parameters
	uint32_t sbtSize = sbtHelper.ComputeSBTSize();

	// Create the SBT on the upload heap. This is required as the helper will use
	// mapping to write the SBT contents. After the SBT compilation it could be
	// copied to the default heap for performance.
	sbtStorage = nv_helpers_dx12::CreateBuffer(
		device.Get(), sbtSize, D3D12_RESOURCE_FLAG_NONE,
		D3D12_RESOURCE_STATE_GENERIC_READ, nv_helpers_dx12::kUploadHeapProps);
	if (!sbtStorage)
	{
		throw std::logic_error("Could not allocate the shader binding table");
	}
	// Compile the SBT from the shader and parameters info
	sbtHelper.Generate(sbtStorage.Get(), stateObjectProps.Get());
}
