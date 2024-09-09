#pragma once
#include "Bindable.h"
#include <wrl/client.h>

class Texture : public Bindable
{
public:
	Texture(Graphics& graphics, const UINT slot, const std::string& fileName);
	virtual void Bind(Graphics& graphics) noexcept override;

	bool HasAlpha() const noexcept;

private:
	bool hasAlpha = false;
	UINT slot;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureView;
};
