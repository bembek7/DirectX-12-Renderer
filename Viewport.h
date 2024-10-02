#pragma once
#include "Bindable.h"

class Viewport : public Bindable
{
public:
	Viewport(const float width, const float height);
	void Bind(Graphics& graphics) noexcept override;

private:
	D3D12_VIEWPORT viewport{};
};
