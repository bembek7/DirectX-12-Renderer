#include "InputLayout.h"
#include "ThrowMacros.h"

InputLayout::InputLayout(Graphics& graphics, const std::vector<D3D11_INPUT_ELEMENT_DESC>& inputElementDescs, const LPVOID BufferPointer, const size_t BufferSize, const std::string& shaderPath)
{
	CHECK_HR(GetDevice(graphics)->CreateInputLayout(&inputElementDescs.front(), (UINT)inputElementDescs.size(), BufferPointer, BufferSize, &inputLayout));
}

void InputLayout::Bind(Graphics& graphics) noexcept
{
	GetContext(graphics)->IASetInputLayout(inputLayout.Get());
}

std::string InputLayout::ResolveID(const std::vector<D3D11_INPUT_ELEMENT_DESC>& inputElementDescs, const LPVOID BufferPointer, const size_t BufferSize, const std::string& shaderPath) noexcept
{
	std::string id = "";
	for (const auto& elementDesc : inputElementDescs)
	{
		id += elementDesc.SemanticName;
	}
	id += shaderPath;
	return id;
}