#include "PixelShader.h"
#include <string>
#include "ThrowMacros.h"
#include <d3dcompiler.h>
#include "Utils.h"

PixelShader::PixelShader(Graphics& graphics, const std::wstring& shaderPath)
{
	Microsoft::WRL::ComPtr<ID3DBlob> blob;
	CHECK_HR(D3DReadFileToBlob(shaderPath.c_str(), &blob));
	CHECK_HR(GetDevice(graphics)->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &pixelShader));
}

void PixelShader::Bind(Graphics& graphics) noexcept
{
	GetContext(graphics)->PSSetShader(pixelShader.Get(), nullptr, 0u);
}

std::string PixelShader::ResolveID(const std::wstring& shaderPath) noexcept
{
	return Utils::WstringToString(shaderPath);
}

void NullPixelShader::Bind(Graphics& graphics) noexcept
{
	GetContext(graphics)->PSSetShader(nullptr, nullptr, 0u);
}