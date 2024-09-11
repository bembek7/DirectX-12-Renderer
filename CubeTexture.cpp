#include "CubeTexture.h"
#include <sstream>
#include "ThrowMacros.h"
#include "dxtex\DirectXTex.h"
#include "Utils.h"

CubeTexture::CubeTexture(Graphics& graphics, const UINT slot, const std::string& path) :
	slot(slot)
{
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;

	DirectX::ScratchImage scratchImages[6];
	for (int i = 0; i < 6; i++)
	{
		const std::wstring texPath = Utils::StringToWstring(path + "\\" + std::to_string(i) + ".png");
		CHECK_HR(DirectX::LoadFromWICFile(texPath.c_str(), DirectX::WIC_FLAGS_NONE, nullptr, scratchImages[i]));
		if (scratchImages[i].GetImage(0, 0, 0)->format != textureDesc.Format)
		{
			DirectX::ScratchImage converted;
			CHECK_HR(DirectX::Convert(*scratchImages[i].GetImage(0, 0, 0), textureDesc.Format, DirectX::TEX_FILTER_DEFAULT, DirectX::TEX_THRESHOLD_DEFAULT, converted));
			scratchImages[i] = std::move(converted);
		}
	}

	textureDesc.Width = (UINT)scratchImages[0].GetMetadata().width;
	textureDesc.Height = (UINT)scratchImages[0].GetMetadata().height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 6;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	D3D11_SUBRESOURCE_DATA data[6]{};
	for (int i = 0; i < 6; i++)
	{
		data[i].pSysMem = scratchImages[i].GetPixels();
		data[i].SysMemPitch = (UINT)scratchImages[i].GetImage(0, 0, 0)->rowPitch;
		data[i].SysMemSlicePitch = 0;
	}

	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2D;
	CHECK_HR(GetDevice(graphics)->CreateTexture2D(&textureDesc, data, &texture2D));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;
	CHECK_HR(GetDevice(graphics)->CreateShaderResourceView(texture2D.Get(), &srvDesc, &textureView));
}

void CubeTexture::Bind(Graphics& graphics) noexcept
{
	GetContext(graphics)->PSSetShaderResources(slot, 1u, textureView.GetAddressOf());
}

std::string CubeTexture::ResolveID(const UINT slot, const std::string& fileName) noexcept
{
	std::stringstream ss;
	ss << slot << fileName;
	return ss.str();
}