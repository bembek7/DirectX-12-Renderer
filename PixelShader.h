#pragma once
#include "Bindable.h"
#include <string>
#include <wrl\client.h>

class PixelShader : public Bindable
{
public:
	PixelShader(Graphics& graphics, const std::wstring& shaderPath);
	virtual void Bind(Graphics& graphics) noexcept override;
	static std::string ResolveID(const std::wstring& shaderPath) noexcept;
private:
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
};

class NullPixelShader : public Bindable
{
public:
	virtual void Bind(Graphics& graphics) noexcept override;
};