#pragma once

class DepthStencilView
{
public:
	DepthStencilView(ID3D11Device* device, const D3D11_TEXTURE2D_DESC& texDesc, const D3D11_DEPTH_STENCIL_VIEW_DESC& viewDesc);
	ID3D11DepthStencilView* Get() noexcept;
	ID3D11Texture2D* GetTexture() noexcept;

private:
	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
};
