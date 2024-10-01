#include "Graphics.h"
#include "DepthStencilView.h"
#include <d3d11.h>
#include "ThrowMacros.h"

DepthStencilView::DepthStencilView(Graphics& graphics, const Usage usage, const UINT width, const UINT height) :
	usage(usage)
{
	D3D11_TEXTURE2D_DESC texDesc = {};
	texDesc.Width = width;
	texDesc.Height = height;
	texDesc.MipLevels = 1u;
	texDesc.ArraySize = 1u;
	texDesc.SampleDesc.Count = 1u;
	texDesc.SampleDesc.Quality = 0u;
	texDesc.Usage = D3D11_USAGE_DEFAULT;

	D3D11_DEPTH_STENCIL_VIEW_DESC viewDesc = {};
	viewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	viewDesc.Texture2D.MipSlice = 0u;

	switch (usage)
	{
	case Usage::DepthStencil:
		texDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
		texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		viewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		break;
	case Usage::Depth:
		texDesc.Format = DXGI_FORMAT_R32_TYPELESS;
		texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
		viewDesc.Format = DXGI_FORMAT_D32_FLOAT;
		break;
	default:
		break;
	}

	CHECK_HR(GetDevice(graphics)->CreateTexture2D(&texDesc, nullptr, &texture));

	CHECK_HR(GetDevice(graphics)->CreateDepthStencilView(texture.Get(), &viewDesc, &depthStencilView));
}

DepthStencilView::DepthStencilView(Graphics& graphics, Microsoft::WRL::ComPtr<ID3D11Texture2D> texture, const UINT face) :
	texture(texture)
{
	D3D11_DEPTH_STENCIL_VIEW_DESC descView = {};
	descView.Format = DXGI_FORMAT_D32_FLOAT;
	descView.Flags = 0;
	descView.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
	descView.Texture2DArray.MipSlice = 0;
	descView.Texture2DArray.ArraySize = 1;
	descView.Texture2DArray.FirstArraySlice = face;
	CHECK_HR(GetDevice(graphics)->CreateDepthStencilView(texture.Get(), &descView, &depthStencilView));
}

void DepthStencilView::Bind(Graphics& graphics) noexcept
{
	//graphics.SetCurrentDepthStenilView(depthStencilView);
}

void DepthStencilView::Update(Graphics& graphics)
{
	Clear(graphics);
}

ID3D11DepthStencilView* DepthStencilView::Get() noexcept
{
	return depthStencilView.Get();
}

ID3D11Texture2D* DepthStencilView::GetTexture() noexcept
{
	return texture.Get();
}

void DepthStencilView::Clear(Graphics& graphics)
{
	UINT clearFlags = 0;
	switch (usage)
	{
	case Usage::DepthStencil:
		clearFlags = D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL;
		break;
	case Usage::Depth:
		clearFlags = D3D11_CLEAR_DEPTH;
		break;
	default:
		break;
	}
	GetContext(graphics)->ClearDepthStencilView(depthStencilView.Get(), clearFlags, 1.0f, 0u);
}