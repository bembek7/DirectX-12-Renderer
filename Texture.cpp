#include "Texture.h"
#include "TexLoader.h"
#include "Graphics.h"

Texture::Texture(Graphics& graphics, const UINT slot, const std::string& fileName, const CD3DX12_CPU_DESCRIPTOR_HANDLE& srvCpuHandle) :
	slot(slot)
{
	auto& texLoader = TexLoader::GetInstance();
	texture = texLoader.GetTexture(graphics, fileName); // be wary that its a shared ptr to com ptr

	// create the descriptor in the heap
	const D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {
		.Format = texture->Get()->GetDesc().Format,
		.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D,
		.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
		.Texture2D{.MipLevels = texture->Get()->GetDesc().MipLevels },
	};
	graphics.device->CreateShaderResourceView(texture->Get(), &srvDesc, srvCpuHandle);
}

UINT Texture::GetSlot() const noexcept
{
	return slot;
}

bool Texture::HasAlpha() const noexcept
{
	return hasAlpha;
}