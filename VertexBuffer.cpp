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

VertexBuffer::VertexBuffer(Graphics& graphics, const std::vector<Vertex>& vertices)
{
	verticesNum = (UINT)vertices.size();

	vertexBuffer = std::move(graphics.GenerateBufferFromData(vertices));

	graphics.ResetCommandListAndAllocator();

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
		.SizeInBytes = verticesNum * (UINT)sizeof(Vertex),
		.StrideInBytes = sizeof(Vertex)
	};
}

void VertexBuffer::Bind(Graphics& graphics) noexcept
{
	GetCommandList(graphics)->IASetVertexBuffers(0, 1, &vertexBufferView);
}

UINT VertexBuffer::GetVerticesNumber() const noexcept
{
	return verticesNum;
}