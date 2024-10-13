#pragma once
#include <wrl/client.h>
#include "d3dx12\d3dx12.h"

class Graphics;

class Texture
{
public:
	Texture(Graphics& graphics, const std::string& fileName, const CD3DX12_CPU_DESCRIPTOR_HANDLE& srvCpuHandle);

	bool HasAlpha() const noexcept;
	//static std::string ResolveID(const UINT slot, const std::string& fileName, std::vector<CD3DX12_ROOT_PARAMETER>& rootParameters) noexcept;
private:
	bool hasAlpha = false;
	std::shared_ptr<Microsoft::WRL::ComPtr<ID3D12Resource>> texture;
	CD3DX12_DESCRIPTOR_RANGE descRange;
};
