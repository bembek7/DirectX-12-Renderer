#pragma once
#include "ThrowMacros.h"
#include <vector>
#include "Bindable.h"
#include <wrl\client.h>

class Graphics;

class IndexBuffer : public Bindable
{
public:
	IndexBuffer(Graphics& graphics, const std::vector<unsigned int>& indices);
	virtual void Bind(Graphics& graphics) noexcept override;
	size_t GetIndicesNumber() const noexcept;
private:
	size_t indicesNum = 0;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
};
