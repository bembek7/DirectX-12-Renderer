#include "Texture.h"
#include "TexLoader.h"
#include "Graphics.h"

Texture::Texture(Graphics& graphics, const std::string& fileName, const CD3DX12_CPU_DESCRIPTOR_HANDLE& srvCpuHandle)
{
	auto& texLoader = TexLoader::GetInstance();
	image = texLoader.GetTexture(graphics, fileName);

	// create the descriptor in the heap
	const D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {
		.Format = image->resource->GetDesc().Format,
		.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D,
		.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
		.Texture2D{.MipLevels = image->resource->GetDesc().MipLevels },
	};
	graphics.GetDevice()->CreateShaderResourceView(image->resource.Get(), &srvDesc, srvCpuHandle);
}

bool Texture::HasAlpha() const noexcept
{
	return image->hasAlpha;
}