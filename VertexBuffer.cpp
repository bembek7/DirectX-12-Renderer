#include "VertexBuffer.h"
#include "ThrowMacros.h"
#include <DirectXMath.h>
#include "d3dx12\d3dx12.h"
#include "Graphics.h"

namespace Dx = DirectX;
namespace Wrl = Microsoft::WRL;

//VertexBuffer::VertexBuffer(Graphics& graphics, const std::vector<float>& vertices, const unsigned int vertexSize) :
//	vertexSize(vertexSize)
//{
//	D3D11_BUFFER_DESC vertexBufferDesc = {};
//	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
//	vertexBufferDesc.ByteWidth = (UINT)vertices.size() * sizeof(float);
//	vertexBufferDesc.StructureByteStride = vertexSize;
//
//	D3D11_SUBRESOURCE_DATA vertexBufferData = {};
//	vertexBufferData.pSysMem = vertices.data();
//
//	CHECK_HR(GetDevice(graphics)->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &vertexBuffer));
//}
//
//void VertexBuffer::Bind(Graphics& graphics) noexcept
//{
//	const UINT stride = vertexSize;
//	const UINT offset = 0u;
//	GetContext(graphics)->IASetVertexBuffers(0u, 1u, vertexBuffer.GetAddressOf(), &stride, &offset);
//}

VertexBuffer::VertexBuffer(Graphics& graphics)
{
	const Vertex vertexData[] =
	{
			{ {  0.00f,  0.50f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } }, // top
			{ {  0.43f, -0.25f, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } }, // right
			{ { -0.43f, -0.25f, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } }, // left
	};

	nVertices = (UINT)std::size(vertexData);

	{
		const CD3DX12_HEAP_PROPERTIES heapProps{ D3D12_HEAP_TYPE_DEFAULT };
		const auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(vertexData));
		CHECK_HR(GetDevice(graphics)->CreateCommittedResource(
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
		CHECK_HR(GetDevice(graphics)->CreateCommittedResource(
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

	graphics.ResetCommandListAndAllocator();
	// copy upload buffer to vertex buffer
	GetCommandList(graphics)->CopyResource(vertexBuffer.Get(), vertexUploadBuffer.Get());
	// transition vertex buffer to vertex buffer state
	{
		const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			vertexBuffer.Get(),
			D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
		GetCommandList(graphics)->ResourceBarrier(1, &barrier);
	}

	// close command list
	CHECK_HR(GetCommandList(graphics)->Close());

	graphics.ExecuteCommandList();

	WaitForQueueFinish(graphics);

	vertexBufferView =
	{
		.BufferLocation = vertexBuffer->GetGPUVirtualAddress(),
		.SizeInBytes = nVertices * (UINT)sizeof(Vertex),
		.StrideInBytes = sizeof(Vertex)
	};
}

void VertexBuffer::Bind(Graphics& graphics) noexcept
{
	GetCommandList(graphics)->IASetVertexBuffers(0, 1, &vertexBufferView);
}

UINT VertexBuffer::GetVerticesNumber() const noexcept
{
	return nVertices;
}