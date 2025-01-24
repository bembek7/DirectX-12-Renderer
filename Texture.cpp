#include "Texture.h"
#include "TexLoader.h"
#include "Graphics.h"

Texture::Texture(Graphics& graphics, const std::string& fileName, const CD3DX12_CPU_DESCRIPTOR_HANDLE& srvCpuHandle)
{
	auto& texLoader = TexLoader::GetInstance();
	image = texLoader.GetTexture(graphics, fileName);

	graphics.CreateSRV(image->resource.Get(), srvCpuHandle);
}

bool Texture::HasAlpha() const noexcept
{
	return image->hasAlpha;
}