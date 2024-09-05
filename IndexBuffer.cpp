#include "IndexBuffer.h"
#include "Bindable.h"
#include "Graphics.h"

IndexBuffer::IndexBuffer(Graphics& graphics, const std::vector<unsigned int>& indices)
{
	indicesNum = indices.size();

	D3D11_BUFFER_DESC indexBufferDesc = {};
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.CPUAccessFlags = 0u;
	indexBufferDesc.MiscFlags = 0u;
	indexBufferDesc.ByteWidth = (UINT)indices.size() * sizeof(unsigned int);
	indexBufferDesc.StructureByteStride = sizeof(unsigned int);

	D3D11_SUBRESOURCE_DATA indexBufferData = {};
	indexBufferData.pSysMem = indices.data();

	CHECK_HR(GetDevice(graphics)->CreateBuffer(&indexBufferDesc, &indexBufferData, &indexBuffer));
}

void IndexBuffer::Bind(Graphics& graphics) noexcept
{
	GetContext(graphics)->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0u);
}

size_t IndexBuffer::GetIndicesNumber() const noexcept
{
	return indicesNum;
}