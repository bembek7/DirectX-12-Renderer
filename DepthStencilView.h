#pragma once
#include "d3dx12\d3dx12.h"

class Graphics;

class DepthStencilView
{
public:
	enum class Usage
	{
		DepthStencil,
		Depth
	};
	DepthStencilView(Graphics& graphics, const Usage usage, const float clearValue, const UINT width, const UINT height);
	CD3DX12_CPU_DESCRIPTOR_HANDLE GetDsvHandle() const noexcept;
	void Clear(ID3D12GraphicsCommandList* const commandList);

private:
	float clearValue = 0.f;
	Usage usage;
	Microsoft::WRL::ComPtr<ID3D12Resource> depthBuffer;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvHeap;
	CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle;
};
