#pragma once
#include "Bindable.h"
#include <string>
#include <wrl\client.h>

class PixelShader : public Bindable
{
public:
	PixelShader(Graphics& graphics, const std::wstring& shaderPath);
	virtual void Bind(Graphics& graphics) noexcept override;
private:
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
};