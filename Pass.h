#pragma once
#include "Bindable.h"
#include <vector>
#include <memory>
#include <DirectXMath.h>

class Graphics;

class Pass
{
public:
	void Execute(Graphics& graphics);
	virtual ~Pass() = default;
protected:
	std::vector<std::unique_ptr<Bindable>> bindables;
	std::vector<std::shared_ptr<Bindable>> sharedBindables;
	DirectX::XMFLOAT4X4 projection{};
};
