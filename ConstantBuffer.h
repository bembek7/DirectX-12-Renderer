#pragma once
#include "ThrowMacros.h"
#include "Bindable.h"
#include "Graphics.h"
#include "d3dx12\d3dx12.h"
#include "BufferLoader.h"

class ConstantBuffer : public Bindable
{
public:
	virtual void Update() = 0;
};

template<typename Structure>
class ConstantBufferCBV : public ConstantBuffer
{
public:
	ConstantBufferCBV(Graphics& graphics, const Structure& data, const UINT rootParameterIndex) :
		bufferData(&data),
		rootParameterIndex(rootParameterIndex)
	{
		const CD3DX12_HEAP_PROPERTIES heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		const CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(data));
		CHECK_HR(graphics.GetDevice()->CreateCommittedResource(
			&heapProperties,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
			nullptr,
			IID_PPV_ARGS(&uploadBuffer)));
	}

	virtual void Update() override
	{
		BYTE* mappedData = nullptr;
		CHECK_HR(uploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&mappedData)));
		memcpy(mappedData, bufferData, sizeof(Structure));
		if (uploadBuffer)
		{
			uploadBuffer->Unmap(0, nullptr);
		}
	}
	virtual void Bind(ID3D12GraphicsCommandList* const commandList) noexcept override
	{
		commandList->SetGraphicsRootConstantBufferView(rootParameterIndex, uploadBuffer->GetGPUVirtualAddress());
	}
private:
	Microsoft::WRL::ComPtr<ID3D12Resource> uploadBuffer;
	const Structure* const bufferData;
	UINT rootParameterIndex;
};

template<typename Structure>
class ConstantBufferConstants : public ConstantBuffer
{
public:
	ConstantBufferConstants(const Structure& data, const UINT rootParameterIndex) :
		bufferData(&data),
		rootParameterIndex(rootParameterIndex)
	{}

	virtual void Update() override
	{
		;
	}
	virtual void Bind(ID3D12GraphicsCommandList* const commandList) noexcept override
	{
		commandList->SetGraphicsRoot32BitConstants(rootParameterIndex, sizeof(Structure) / 4, bufferData, 0);
	}
private:
	const Structure* const bufferData;
	UINT rootParameterIndex;
};