#include "Texture.h"
#include "WICTextureLoader.h"
#include "Utils.h"
#include <wrl/client.h>
#include "ThrowMacros.h"
#include "dxtex\DirectXTex.h"

Texture::Texture(Graphics& graphics, const UINT slot, const std::string& fileName) :
	slot(slot)
{
	id = fileName;

	Microsoft::WRL::ComPtr<ID3D11Resource> texture;
	DirectX::ScratchImage scratchImage;
	CHECK_HR(DirectX::LoadFromWICFile(Utils::StringToWstring(fileName).c_str(), DirectX::WIC_FLAGS_NONE, nullptr, scratchImage));

	hasAlpha = !scratchImage.IsAlphaAllOpaque();

	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = (UINT)scratchImage.GetMetadata().width;
	textureDesc.Height = (UINT)scratchImage.GetMetadata().height;
	textureDesc.MipLevels = 0;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	if (scratchImage.GetImage(0, 0, 0)->format != textureDesc.Format)
	{
		DirectX::ScratchImage converted;
		CHECK_HR(DirectX::Convert(*scratchImage.GetImage(0, 0, 0), textureDesc.Format, DirectX::TEX_FILTER_DEFAULT, DirectX::TEX_THRESHOLD_DEFAULT, converted));
		scratchImage = std::move(converted);
	}

	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2D;
	CHECK_HR(GetDevice(graphics)->CreateTexture2D(&textureDesc, nullptr, &texture2D));

	GetContext(graphics)->UpdateSubresource(texture2D.Get(), 0u, nullptr, scratchImage.GetPixels(), (UINT)scratchImage.GetMetadata().width * sizeof(unsigned int), 0u);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;
	CHECK_HR(GetDevice(graphics)->CreateShaderResourceView(texture2D.Get(), &srvDesc, &textureView));

	GetContext(graphics)->GenerateMips(textureView.Get());
}

void Texture::Bind(Graphics& graphics) noexcept
{
	GetContext(graphics)->PSSetShaderResources(slot, 1u, textureView.GetAddressOf());
}

bool Texture::HasAlpha() const noexcept
{
	return hasAlpha;
}