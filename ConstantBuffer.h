#pragma once
#include "ThrowMacros.h"
#include "Bindable.h"
#include "Graphics.h"

enum class BufferType
{
	Pixel,
	Vertex
};

template<typename Structure>
class ConstantBuffer : public Bindable
{
public:
	ConstantBuffer(Graphics& graphics, const Structure& data, const BufferType bufferType, const UINT slot) :
		bufferData(&data),
		bufferType(bufferType),
		slot(slot)
	{
		D3D11_BUFFER_DESC bufferDesc{};
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDesc.ByteWidth = sizeof(data);
		D3D11_SUBRESOURCE_DATA subData = {};
		subData.pSysMem = &data;

		CHECK_HR(GetDevice(graphics)->CreateBuffer(&bufferDesc, &subData, &constantBuffer));
	}
	virtual void Update(Graphics& graphics) override
	{
		D3D11_MAPPED_SUBRESOURCE mappedSubresource;
		CHECK_HR(GetContext(graphics)->Map(constantBuffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &mappedSubresource));
		memcpy(mappedSubresource.pData, bufferData, sizeof(*bufferData));
		GetContext(graphics)->Unmap(constantBuffer.Get(), 0u);
	}
	virtual void Bind(Graphics& graphics) noexcept override
	{
		switch (bufferType)
		{
		case BufferType::Pixel:
			GetContext(graphics)->PSSetConstantBuffers(slot, 1u, constantBuffer.GetAddressOf());
			break;
		case BufferType::Vertex:
			GetContext(graphics)->VSSetConstantBuffers(slot, 1u, constantBuffer.GetAddressOf());
			break;
		}
	}
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;
	const Structure* const bufferData;
	UINT slot;
	BufferType bufferType;
};