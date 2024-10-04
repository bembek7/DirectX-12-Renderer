#pragma once
#include "ThrowMacros.h"
#include "Bindable.h"
#include "Graphics.h"
#include "d3dx12\d3dx12.h"

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
		D3D12_SHADER_VISIBILITY shaderVisibility;
		switch (bufferType)
		{
		case BufferType::Pixel:
			shaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
			break;
		case BufferType::Vertex:
			shaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
			break;
		}
		CD3DX12_ROOT_PARAMETER rootParameter{};
		rootParameter.InitAsConstants(sizeof(data) / 4, slot, 0, shaderVisibility);
		GetRootParameters(graphics).push_back(std::move(rootParameter));
		rootParameterIndex = (UINT)GetRootParameters(graphics).size() - 1;
	}
	virtual void Update(Graphics& graphics) override
	{
		;
	}
	virtual void Bind(Graphics& graphics) noexcept override
	{
		GetCommandList(graphics)->SetGraphicsRoot32BitConstants(rootParameterIndex, sizeof(*bufferData) / 4, bufferData, 0);
	}
private:
	const Structure* const bufferData;
	UINT slot;
	UINT rootParameterIndex;
	BufferType bufferType;
};