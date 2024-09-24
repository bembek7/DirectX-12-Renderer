#pragma once
#include <wrl\client.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include "Bindable.h"

class RenderTargetView : public Bindable
{
public:
	RenderTargetView(Graphics& graphics);
	virtual void Bind(Graphics& graphics) noexcept override;
	virtual void Update(Graphics& graphics) override;
private:
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;
	void Clear(Graphics& graphics, const FLOAT color[4]);
};

class NullRenderTargetView : public Bindable
{
public:
	virtual void Bind(Graphics& graphics) noexcept override;
};