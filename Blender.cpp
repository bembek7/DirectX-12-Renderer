#include "Blender.h"
#include "ThrowMacros.h"

Blender::Blender(Graphics& graphics, const bool blending) :
	blending(blending)
{
	D3D11_BLEND_DESC blendDesc = {};
	auto& brt = blendDesc.RenderTarget[0];
	if (blending)
	{
		brt.BlendEnable = TRUE;
		brt.SrcBlend = D3D11_BLEND_SRC_ALPHA;
		brt.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		brt.BlendOp = D3D11_BLEND_OP_ADD;
		brt.SrcBlendAlpha = D3D11_BLEND_ZERO;
		brt.DestBlendAlpha = D3D11_BLEND_ZERO;
		brt.BlendOpAlpha = D3D11_BLEND_OP_ADD;
		brt.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	}
	else
	{
		brt.BlendEnable = FALSE;
		brt.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	}

	CHECK_HR(GetDevice(graphics)->CreateBlendState(&blendDesc, &blenderState));
}

void Blender::Bind(Graphics& graphics) noexcept
{
	GetContext(graphics)->OMSetBlendState(blenderState.Get(), nullptr, 0xFFFFFFFFu);
}

std::string Blender::ResolveID(const bool blending) noexcept
{
	return (blending) ? "true" : "false";
}