#include "DepthCubeTexture.h"
#include "ThrowMacros.h"

DepthCubeTexture::DepthCubeTexture(Graphics& graphics, const UINT slot, const UINT faceSize) :
	faceSize(faceSize)
{
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	textureDesc.Width = faceSize;
	textureDesc.Height = faceSize;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 6;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2D;
	CHECK_HR(GetDevice(graphics)->CreateTexture2D(&textureDesc, nullptr, &texture2D));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;
	CHECK_HR(GetDevice(graphics)->CreateShaderResourceView(texture2D.Get(), &srvDesc, &textureView));

	for (UINT face = 0; face < 6; face++)
	{
		depthBuffers.push_back(std::make_unique<DepthStencilView>(graphics, texture2D, face));
	}
}

void DepthCubeTexture::Bind(Graphics& graphics) noexcept
{
	GetContext(graphics)->PSSetShaderResources(slot, 1u, textureView.GetAddressOf());
}

void DepthCubeTexture::Clear(Graphics& graphics) noexcept
{
	for (const auto& depthBuffer : depthBuffers)
	{
		depthBuffer->Update(graphics);
	}
}

DepthStencilView* DepthCubeTexture::GetDepthBuffer(const unsigned int face)
{
	return depthBuffers[face].get();
}

UINT DepthCubeTexture::GetFaceSize() const noexcept
{
	return faceSize;
}