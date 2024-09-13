#include "ShadowRasterizer.h"
#include "ThrowMacros.h"

ShadowRasterizer::ShadowRasterizer(Graphics& graphics, const D3D11_CULL_MODE cullMode)
{
	D3D11_RASTERIZER_DESC rasterizerDesc = {};
	rasterizerDesc.CullMode = cullMode;
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.DepthClipEnable = true;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.SlopeScaledDepthBias = 0.0f;
	rasterizerDesc.DepthBiasClamp = 0.1f;

	CHECK_HR(GetDevice(graphics)->CreateRasterizerState(&rasterizerDesc, &rasterizerState));
}

std::string ShadowRasterizer::ResolveID(const D3D11_CULL_MODE cullMode) noexcept
{
	return Rasterizer::ResolveID(cullMode);
}