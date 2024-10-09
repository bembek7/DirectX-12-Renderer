#pragma once
#include <wrl/client.h>
#include "d3dx12\d3dx12.h"

class Graphics;

class Texture
{
public:
	Texture(Graphics& graphics, const UINT slot, const std::string& fileName, std::vector<CD3DX12_ROOT_PARAMETER>& rootParameters);
	void Bind(Graphics& graphics, const CD3DX12_GPU_DESCRIPTOR_HANDLE& srvGpuHandle) noexcept;

	bool HasAlpha() const noexcept;
	//static std::string ResolveID(const UINT slot, const std::string& fileName, std::vector<CD3DX12_ROOT_PARAMETER>& rootParameters) noexcept;
private:
	bool hasAlpha = false;
	UINT slot;
	UINT rootParameterIndex;
	Microsoft::WRL::ComPtr<ID3D12Resource> texture;
	CD3DX12_DESCRIPTOR_RANGE descRange;
};
