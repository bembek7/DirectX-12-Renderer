#pragma once
#include "Bindable.h"
#include <vector>
#include <wrl\client.h>

class InputLayout : public Bindable
{
public:
	InputLayout(Graphics& graphics, const std::vector<D3D11_INPUT_ELEMENT_DESC>& inputElementDescs, const LPVOID BufferPointer, const size_t BufferSize, const std::string& shaderPath);
	virtual void Bind(Graphics& graphics) noexcept override;
private:
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
};
