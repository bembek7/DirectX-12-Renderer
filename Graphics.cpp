#include "Graphics.h"
#include "ThrowMacros.h"
#include <d3dcommon.h>
#include <numbers>
#include "Camera.h"
#include "DepthCubeTexture.h"
#include <d3dcompiler.h>
#include "Utils.h"

namespace Dx = DirectX;
namespace Wrl = Microsoft::WRL;

Graphics::Graphics(const HWND& hWnd, const float windowWidth, const float windowHeight) :
	windowWidth(windowWidth),
	windowHeight(windowHeight)
{
	LoadPipeline(hWnd);
	LoadAssets();
}

void Graphics::OnUpdate()
{
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

	WaitForPreviousFrame();
}

void Graphics::OnDestroy()
{
	CHECK_HR(commandQueue->Signal(fence.Get(), ++fenceValue));
	CHECK_HR(fence->SetEventOnCompletion(fenceValue, fenceEvent));
	if (WaitForSingleObject(fenceEvent, 2000) == WAIT_FAILED)
	{
		CHECK_HR(GetLastError());
	}

	CloseHandle(fenceEvent);
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
	rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(
		D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

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
	CHECK_HR(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator.Get(), pipelineState.Get(), IID_PPV_ARGS(&commandList)));

	// Command lists are created in the recording state, but there is nothing
	// to record yet. The main loop expects it to be closed, so close it now.
	CHECK_HR(commandList->Close());

	// Create synchronization objects and wait until assets have been uploaded to the GPU.
	{
		fenceValue = 0;
		CHECK_HR(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));

		// fence signalling event
		HANDLE fenceEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
		if (!fenceEvent)
		{
			CHECK_HR(GetLastError());
			throw std::runtime_error{ "Failed to create fence event" };
		}
	}

	// Create the vertex buffer.
	{
		const Vertex vertexData[] =
		{
				{ {  0.00f,  0.50f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } }, // top
				{ {  0.43f, -0.25f, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } }, // right
				{ { -0.43f, -0.25f, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } }, // left
		};

		nVertices = (UINT)std::size(vertexData);

		// Note: using upload heaps to transfer static data like vert buffers is not
		// recommended. Every time the GPU needs it, the upload heap will be marshalled
		// over. Please read up on Default Heap usage. An upload heap is used here for
		// code simplicity and because there are very few verts to actually transfer.
		{
			const CD3DX12_HEAP_PROPERTIES heapProps{ D3D12_HEAP_TYPE_DEFAULT };
			const auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(vertexData));
			CHECK_HR(device->CreateCommittedResource(
				&heapProps,
				D3D12_HEAP_FLAG_NONE,
				&resourceDesc,
				D3D12_RESOURCE_STATE_COMMON, // if created with copy_dest raises warning
				nullptr, IID_PPV_ARGS(&vertexBuffer)
			));
		}
		// create committed resource for cpu upload of vertex data
		Wrl::ComPtr<ID3D12Resource> vertexUploadBuffer;
		{
			const CD3DX12_HEAP_PROPERTIES heapProps{ D3D12_HEAP_TYPE_UPLOAD };
			const auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(vertexData));
			CHECK_HR(device->CreateCommittedResource(
				&heapProps,
				D3D12_HEAP_FLAG_NONE,
				&resourceDesc,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr, IID_PPV_ARGS(&vertexUploadBuffer)
			));
		}

		// copy array of vertex data to upload buffer
		{
			Vertex* mappedVertexData = nullptr;
			CHECK_HR(vertexUploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&mappedVertexData)));
			std::ranges::copy(vertexData, mappedVertexData);
			vertexUploadBuffer->Unmap(0, nullptr);
		}

		// reset command list and allocator
		CHECK_HR(commandAllocator->Reset());
		CHECK_HR(commandList->Reset(commandAllocator.Get(), nullptr));
		// copy upload buffer to vertex buffer
		commandList->CopyResource(vertexBuffer.Get(), vertexUploadBuffer.Get());
		// transition vertex buffer to vertex buffer state
		{
			const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
				vertexBuffer.Get(),
				D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
			commandList->ResourceBarrier(1, &barrier);
		}
		// close command list
		CHECK_HR(commandList->Close());
		// submit command list to queue as array with single element
		ID3D12CommandList* const commandLists[] = { commandList.Get() };
		commandQueue->ExecuteCommandLists((UINT)std::size(commandLists), commandLists);
		// insert fence to detect when upload is complete
		CHECK_HR(commandQueue->Signal(fence.Get(), ++fenceValue));
		CHECK_HR(fence->SetEventOnCompletion(fenceValue, fenceEvent));
		if (WaitForSingleObject(fenceEvent, INFINITE) == WAIT_FAILED)
		{
			CHECK_HR(GetLastError());
		}
	}

	vertexBufferView =
	{
		.BufferLocation = vertexBuffer->GetGPUVirtualAddress(),
		.SizeInBytes = nVertices * (UINT)sizeof(Vertex),
		.StrideInBytes = sizeof(Vertex)
	};

	// create root signature
	{
		// define empty root signature
		CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
		rootSignatureDesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
		// serialize root signature
		Wrl::ComPtr<ID3DBlob> signatureBlob;
		Wrl::ComPtr<ID3DBlob> errorBlob;
		if (const auto hr = D3D12SerializeRootSignature(
			&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1,
			&signatureBlob, &errorBlob); FAILED(hr))
		{
			if (errorBlob)
			{
				auto errorBufferPtr = static_cast<const char*>(errorBlob->GetBufferPointer());
				throw std::runtime_error(errorBufferPtr);
			}
			CHECK_HR(hr);
		}
		// Create the root signature.
		CHECK_HR(device->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
			signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature)));
	}

	// creating pipeline state object
	{
		// static declaration of pso stream structure
		struct PipelineStateStream
		{
			CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE RootSignature;
			CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT InputLayout;
			CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY PrimitiveTopologyType;
			CD3DX12_PIPELINE_STATE_STREAM_VS VS;
			CD3DX12_PIPELINE_STATE_STREAM_PS PS;
			CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS RTVFormats;
		} pipelineStateStream;

		// define the Vertex input layout
		const D3D12_INPUT_ELEMENT_DESC inputLayout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		};

		// Load the vertex shader.
		Wrl::ComPtr<ID3DBlob> vertexShaderBlob;
		CHECK_HR(D3DReadFileToBlob(L"VertexShader.cso", &vertexShaderBlob));

		// Load the pixel shader.
		Wrl::ComPtr<ID3DBlob> pixelShaderBlob;
		CHECK_HR(D3DReadFileToBlob(L"PixelShader.cso", &pixelShaderBlob));

		// filling pso structure
		pipelineStateStream.RootSignature = rootSignature.Get();
		pipelineStateStream.InputLayout = { inputLayout, (UINT)std::size(inputLayout) };
		pipelineStateStream.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		pipelineStateStream.VS = CD3DX12_SHADER_BYTECODE(vertexShaderBlob.Get());
		pipelineStateStream.PS = CD3DX12_SHADER_BYTECODE(pixelShaderBlob.Get());
		pipelineStateStream.RTVFormats = {
			.RTFormats{ DXGI_FORMAT_R8G8B8A8_UNORM },
			.NumRenderTargets = 1,
		};

		// building the pipeline state object
		const D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc = {
			sizeof(PipelineStateStream), &pipelineStateStream
		};
		CHECK_HR(device->CreatePipelineState(&pipelineStateStreamDesc, IID_PPV_ARGS(&pipelineState)));
	}

	// define scissor rect
	scissorRect = { 0, 0, LONG_MAX, LONG_MAX };

	// define viewport
	viewport = { 0.0f, 0.0f, windowWidth, windowHeight };
}

void Graphics::PopulateCommandList()
{
	curBackBufferIndex = swapChain->GetCurrentBackBufferIndex();
	// select current buffer to render to
	auto& backBuffer = renderTargets[curBackBufferIndex];

	// reset command list and allocator
	CHECK_HR(commandAllocator->Reset());
	CHECK_HR(commandList->Reset(commandAllocator.Get(), nullptr));
	// get rtv handle for the buffer used in this frame
	const CD3DX12_CPU_DESCRIPTOR_HANDLE rtv
	{
		rtvHeap->GetCPUDescriptorHandleForHeapStart(),
		(INT)curBackBufferIndex, rtvDescriptorSize
	};
	// clear the render target
	{
		// transition buffer resource to render target state
		const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			backBuffer.Get(),
			D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		commandList->ResourceBarrier(1, &barrier);
		// calculate clear color
		const FLOAT clearColor[] =
		{
			0.13f,
			0.05f,
			0.05f,
			1.0f
		};
		// clear rtv
		commandList->ClearRenderTargetView(rtv, clearColor, 0, nullptr);
	}
	// set pipeline state
	commandList->SetPipelineState(pipelineState.Get());
	commandList->SetGraphicsRootSignature(rootSignature.Get());
	// configure IA
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
	// configure RS
	commandList->RSSetViewports(1, &viewport);
	commandList->RSSetScissorRects(1, &scissorRect);
	// bind render target
	commandList->OMSetRenderTargets(1, &rtv, TRUE, nullptr);
	// draw the geometry
	commandList->DrawInstanced(nVertices, 1, 0, 0);
	// prepare buffer for presentation by transitioning to present state
	{
		const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			backBuffer.Get(),
			D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		commandList->ResourceBarrier(1, &barrier);
	}

	CHECK_HR(commandList->Close());
}

void Graphics::WaitForPreviousFrame()
{
	// WAITING FOR THE FRAME TO COMPLETE BEFORE CONTINUING IS NOT BEST PRACTICE.
	// This is code implemented as such for simplicity. More advanced samples
	// illustrate how to use fences for efficient resource usage.

	// Signal and increment the fence value.
	CHECK_HR(commandQueue->Signal(fence.Get(), ++fenceValue));
	CHECK_HR(fence->SetEventOnCompletion(fenceValue, fenceEvent));
	if (WaitForSingleObject(fenceEvent, INFINITE) == WAIT_FAILED)
	{
		CHECK_HR(GetLastError());
	}
}

float Graphics::GetWindowWidth() const noexcept
{
	return windowWidth;
}

float Graphics::GetWindowHeight() const noexcept
{
	return windowHeight;
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