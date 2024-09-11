#pragma once
#include "Bindable.h"
#include <wrl\client.h>

class DepthStencilState : public Bindable
{
public:
	DepthStencilState(Graphics& graphics, const D3D11_DEPTH_STENCIL_DESC& desc);
	void Bind(Graphics& graphics) noexcept override;
private:
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;
};
