#include "PixelShader.h"
#include <string>
#include "ThrowMacros.h"
#include <d3dcompiler.h>
#include "Utils.h"

PixelShader::PixelShader(Graphics& graphics, const std::wstring& shaderPath)
{
	if (!shaderPath.empty())
	{
		id = Utils::WstringToString(shaderPath);
		Microsoft::WRL::ComPtr<ID3DBlob> blob;
		CHECK_HR(D3DReadFileToBlob(shaderPath.c_str(), &blob));
		CHECK_HR(GetDevice(graphics)->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &pixelShader));
	}
	else
	{
		id = "None";
	}
}

void PixelShader::Bind(Graphics& graphics) noexcept
{
	if (id != "None")
	{
		GetContext(graphics)->PSSetShader(pixelShader.Get(), nullptr, 0u);
	}
	else
	{
		GetContext(graphics)->PSSetShader(nullptr, nullptr, 0u);
	}
}