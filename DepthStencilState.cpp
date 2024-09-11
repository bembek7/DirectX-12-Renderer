#include "DepthStencilState.h"
#include "ThrowMacros.h"

DepthStencilState::DepthStencilState(Graphics& graphics, const D3D11_DEPTH_STENCIL_DESC& desc)
{
	CHECK_HR(GetDevice(graphics)->CreateDepthStencilState(&desc, &depthStencilState));
}

void DepthStencilState::Bind(Graphics& graphics) noexcept
{
	GetContext(graphics)->OMSetDepthStencilState(depthStencilState.Get(), 1u);
}