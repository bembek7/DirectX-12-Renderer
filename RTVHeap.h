#pragma once
#include "d3dx12/d3dx12.h"

class Graphics;

class RTVHeap
{
public:
	RTVHeap(Graphics& graphics, const UINT buffersNum, const D3D12_RENDER_TARGET_VIEW_DESC* const rtvDescs);

	CD3DX12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(const INT bufferIndex = 0) noexcept;
	ID3D12Resource* GetRenderTarget(const UINT rtIndex) noexcept;
private:
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap;
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> renderTargets;
	UINT rtvDescriptorSize;
};

