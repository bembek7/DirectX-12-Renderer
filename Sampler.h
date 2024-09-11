#pragma once
#include "Bindable.h"
#include <wrl\client.h>

class Sampler : public Bindable
{
public:
	enum class Mode
	{
		Biliniear,
		Anisotropic,
		Comparison
	};
	Sampler(Graphics& graphics, const UINT slot, const Mode samplerMode);

	void Bind(Graphics& graphics) noexcept override;
	static std::string ResolveID(const UINT slot, const Mode samplerMode) noexcept;
private:
	UINT slot;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler;
};
