#pragma once

class DepthStencilView
{
public:
	enum class Usage
	{
		DepthStencil,
		Depth
	};
	DepthStencilView(ID3D11Device* const device, const Usage usage, const UINT width, const UINT height);
	DepthStencilView(ID3D11Device* const device, Microsoft::WRL::ComPtr<ID3D11Texture2D> texture, const UINT face);
	ID3D11DepthStencilView* Get() noexcept;
	ID3D11Texture2D* GetTexture() noexcept;

	void Clear(ID3D11DeviceContext* const context);

private:
	Usage usage;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
};
