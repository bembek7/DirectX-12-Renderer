#pragma once
#include "d3dx12\d3dx12.h"

class Graphics;

class DepthStencilView
{
public:
	enum class Usage
	{
		DepthStencil,
		Depth,
		DepthCube
	};
	DepthStencilView(Graphics& graphics, const Usage usage, const float clearValue, const UINT width, const UINT height);
	CD3DX12_CPU_DESCRIPTOR_HANDLE GetDsvHandle(const UINT bufferIndex = 0) noexcept;
	void Clear(ID3D12GraphicsCommandList* const commandList);
	ID3D12Resource* GetBuffer() noexcept;

private:
	float clearValue = 0.f;
	Usage usage;
	Microsoft::WRL::ComPtr<ID3D12Resource> depthBuffer;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvHeap;
	UINT descSize = 0;
};
