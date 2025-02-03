#pragma once
#include "d3dx12\d3dx12.h"

class Bindable
{
public:
	virtual void Bind(ID3D12GraphicsCommandList* const commandList) noexcept = 0;
	virtual ~Bindable() = default;
};
