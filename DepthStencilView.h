#pragma once
#include "Bindable.h"

class DepthStencilView : public Bindable
{
public:
	enum class Usage
	{
		DepthStencil,
		Depth
	};
	DepthStencilView(Graphics& graphics, const Usage usage, const UINT width, const UINT height);
	DepthStencilView(Graphics& graphics, Microsoft::WRL::ComPtr<ID3D11Texture2D> texture, const UINT face);
	void Bind(Graphics& graphics) noexcept override;
	virtual void Update(Graphics& graphics) override;
	ID3D11DepthStencilView* Get() noexcept;
	ID3D11Texture2D* GetTexture() noexcept;

private:
	void Clear(Graphics& graphics);

private:
	Usage usage;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
};
