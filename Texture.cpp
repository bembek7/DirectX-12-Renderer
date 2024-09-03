#include "Texture.h"
#include "WICTextureLoader.h"
#include "Utils.h"
#include <wrl/client.h>
#include "ThrowMacros.h"

Texture::Texture(Graphics& graphics, const UINT slot, const std::string& fileName) :
	slot(slot)
{
	id = fileName;
	CreateWICTextureFromFile(GetDevice(graphics), GetContext(graphics), Utils::StringToWstring(fileName).c_str(), &texture, &textureView);
}

void Texture::Bind(Graphics& graphics) noexcept
{
	GetContext(graphics)->PSSetShaderResources(slot, 1u, textureView.GetAddressOf());
}