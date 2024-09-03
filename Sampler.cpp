#include "Sampler.h"
#include "ThrowMacros.h"

Sampler::Sampler(Graphics& graphics, const UINT slot, const D3D11_SAMPLER_DESC& samplerDesc) :
	slot(slot)
{
	CHECK_HR(GetDevice(graphics)->CreateSamplerState(&samplerDesc, &sampler));
}

void Sampler::Bind(Graphics& graphics) noexcept
{
	GetContext(graphics)->PSSetSamplers(slot, 1u, sampler.GetAddressOf());
}