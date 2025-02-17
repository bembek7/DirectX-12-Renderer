#include "Rasterizer.h"
#include "ThrowMacros.h"

Rasterizer::Rasterizer(Graphics& graphics, const D3D11_CULL_MODE cullMode)
{
	D3D11_RASTERIZER_DESC rasterizerDesc = {};
	rasterizerDesc.CullMode = cullMode;
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.DepthClipEnable = true;

	CHECK_HR(GetDevice(graphics)->CreateRasterizerState(&rasterizerDesc, &rasterizerState));
}

void Rasterizer::Bind(Graphics& graphics) noexcept
{
	GetContext(graphics)->RSSetState(rasterizerState.Get());
}

std::string Rasterizer::ResolveID(const D3D11_CULL_MODE cullMode) noexcept
{
	std::string id;
	switch (cullMode)
	{
	case D3D11_CULL_NONE:
		id = "cullNone";
		break;
	case D3D11_CULL_FRONT:
		id = "cullFront";
		break;
	case D3D11_CULL_BACK:
		id = "cullBack";
		break;
	default:
		break;
	}
	return id;
}