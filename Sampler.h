#pragma once
#include "Bindable.h"
#include <wrl\client.h>

class Sampler : public Bindable
{
public:
	Sampler(Graphics& graphics, const UINT slot, const D3D11_SAMPLER_DESC& samplerDesc);

	void Bind(Graphics& graphics) noexcept override;

private:
	UINT slot;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler;
};
