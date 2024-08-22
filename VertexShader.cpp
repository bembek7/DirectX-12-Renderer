#include "VertexShader.h"
#include <d3dcompiler.h>
#include <string>
#include "ThrowMacros.h"
#include "Graphics.h"
#include "Utils.h"

VertexShader::VertexShader(Graphics& graphics, const std::wstring& shaderPath)
{
	id = WstringToString(shaderPath);
	CHECK_HR(D3DReadFileToBlob(shaderPath.c_str(), &blob));
	CHECK_HR(GetDevice(graphics)->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &vertexShader));
}

void VertexShader::Bind(Graphics& graphics) noexcept
{
	GetContext(graphics)->VSSetShader(vertexShader.Get(), nullptr, 0u);
}

LPVOID VertexShader::GetBufferPointer() const noexcept
{
	return blob->GetBufferPointer();
}

size_t VertexShader::GetBufferSize() const noexcept
{
	return blob->GetBufferSize();
}