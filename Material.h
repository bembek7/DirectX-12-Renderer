#pragma once
#include "BetterWindows.h"
#include "Bindable.h"
#include <vector>
#include <memory>
#include <DirectXMath.h>

struct aiMaterial;

class Material
{
public:
	Material(Graphics& graphics, const aiMaterial* const assignedMaterial);
	void Bind(Graphics& graphics) noexcept;

private:
	std::vector<std::unique_ptr<Bindable>> bindables;
	std::vector<std::shared_ptr<Bindable>> sharedBindables;

	struct ColorBuffer
	{
		ColorBuffer() = default;
		ColorBuffer(const DirectX::XMFLOAT4 newColor) :
			color(newColor)
		{}
		DirectX::XMFLOAT4 color;
	};
	ColorBuffer colorBuffer = { { 1.0f, 1.0f, 1.0f, 1.0f } };
};
