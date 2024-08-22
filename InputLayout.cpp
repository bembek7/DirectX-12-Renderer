#include "InputLayout.h"
#include "ThrowMacros.h"

InputLayout::InputLayout(Graphics& graphics, const std::vector<D3D11_INPUT_ELEMENT_DESC>& inputElementDescs, const LPVOID BufferPointer, const size_t BufferSize, const std::string& shaderPath)
{
	for (const auto& elementDesc : inputElementDescs)
	{
		id += elementDesc.SemanticName;
	}
	id += shaderPath;
	CHECK_HR(GetDevice(graphics)->CreateInputLayout(&inputElementDescs.front(), (UINT)inputElementDescs.size(), BufferPointer, BufferSize, &inputLayout));
}

void InputLayout::Bind(Graphics& graphics) noexcept
{
	GetContext(graphics)->IASetInputLayout(inputLayout.Get());
}