#include "DepthStencilState.h"
#include "ThrowMacros.h"

DepthStencilState::DepthStencilState(Graphics& graphics, const Usage usage)
{
	D3D11_DEPTH_STENCIL_DESC desc = {};
	desc.DepthEnable = TRUE;

	switch (usage)
	{
	case Usage::Regular:
		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		desc.DepthFunc = D3D11_COMPARISON_LESS;
		break;
	case Usage::Skybox:
		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		break;
	default:
		break;
	}
	CHECK_HR(GetDevice(graphics)->CreateDepthStencilState(&desc, &depthStencilState));
}

void DepthStencilState::Bind(Graphics& graphics) noexcept
{
	GetContext(graphics)->OMSetDepthStencilState(depthStencilState.Get(), 1u);
}