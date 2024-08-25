#include "Graphics.h"
#include "DepthStencilView.h"
#include <d3d11.h>
#include <wrl\client.h>
#include "ThrowMacros.h"

DepthStencilView::DepthStencilView(ID3D11Device* device, const D3D11_TEXTURE2D_DESC& texDesc, const D3D11_DEPTH_STENCIL_VIEW_DESC& viewDesc)
{
	CHECK_HR(device->CreateTexture2D(&texDesc, nullptr, &texture));

	CHECK_HR(device->CreateDepthStencilView(texture.Get(), &viewDesc, &depthStencilView));
}

ID3D11DepthStencilView* DepthStencilView::Get() noexcept
{
	return depthStencilView.Get();
}

ID3D11Texture2D* DepthStencilView::GetTexture() noexcept
{
	return texture.Get();
}
