#pragma once
#include "Bindable.h"
#include "d3dx12\d3dx12.h"

class Viewport : public Bindable
{
public:
	Viewport(const float width, const float height);
	void Bind(Graphics& graphics) noexcept override;

private:
	CD3DX12_VIEWPORT viewport;
};
