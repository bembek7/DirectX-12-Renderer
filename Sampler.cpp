#include "Sampler.h"
#include "ThrowMacros.h"

Sampler::Sampler(Graphics& graphics, const UINT slot, const Mode samplerMode) :
	slot(slot)
{
	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MinLOD = 0.0f;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	switch (samplerMode)
	{
	case Mode::Biliniear:
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		break;
	case Mode::Anisotropic:
		samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
		samplerDesc.MaxAnisotropy = D3D11_REQ_MAXANISOTROPY;
		break;
	case Mode::Comparison:
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
		samplerDesc.BorderColor[0] = 1.0f;
		samplerDesc.BorderColor[1] = 1.0f;
		samplerDesc.BorderColor[2] = 1.0f;
		samplerDesc.BorderColor[3] = 1.0f;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
		samplerDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
		break;
	default:
		break;
	}
	CHECK_HR(GetDevice(graphics)->CreateSamplerState(&samplerDesc, &sampler));
}

void Sampler::Bind(Graphics& graphics) noexcept
{
	GetContext(graphics)->PSSetSamplers(slot, 1u, sampler.GetAddressOf());
}

std::string Sampler::ResolveID(const UINT slot, const Mode samplerMode) noexcept
{
	std::string mode;
	switch (samplerMode)
	{
	case Mode::Biliniear:
		mode = "Biliniear";
		break;
	case Mode::Anisotropic:
		mode = "Anisotropic";
		break;
	case Mode::Comparison:
		mode = "Comparison";
		break;
	default:
		break;
	}
	std::stringstream ss;
	ss << slot << mode;
	return ss.str();
}