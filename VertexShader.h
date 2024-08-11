#pragma once
#include "Bindable.h"
#include <string>
#include <wrl\client.h>

class VertexShader : public Bindable
{
public:
	VertexShader(Graphics& graphics, std::wstring shaderPath);
	virtual void Bind(Graphics& graphics) noexcept override;
	LPVOID GetBufferPointer() noexcept;
	size_t GetBufferSize() const noexcept;
private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3DBlob> blob;
};