#pragma once
#include "Bindable.h"
class ScissorRectangle : public Bindable
{
public:
	void Bind(ID3D12GraphicsCommandList* const commandList) noexcept override;
private:
	const D3D12_RECT scissorRect = { 0, 0, LONG_MAX, LONG_MAX };
};
