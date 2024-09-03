#include "VertexBuffer.h"
#include "ThrowMacros.h"
#include <DirectXMath.h>

VertexBuffer::VertexBuffer(Graphics& graphics, const std::vector<float>& vertices, const unsigned int vertexSize) :
	vertexSize(vertexSize)
{
	D3D11_BUFFER_DESC vertexBufferDesc = {};
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = (UINT)vertices.size() * sizeof(float);
	vertexBufferDesc.StructureByteStride = vertexSize;

	D3D11_SUBRESOURCE_DATA vertexBufferData = {};
	vertexBufferData.pSysMem = vertices.data();

	CHECK_HR(GetDevice(graphics)->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &vertexBuffer));
}

void VertexBuffer::Bind(Graphics& graphics) noexcept
{
	const UINT stride = vertexSize;
	const UINT offset = 0u;
	GetContext(graphics)->IASetVertexBuffers(0u, 1u, vertexBuffer.GetAddressOf(), &stride, &offset);
}