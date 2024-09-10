#pragma once
#include "Rasterizer.h"

class ShadowRasterizer : public Rasterizer
{
public:
	ShadowRasterizer(Graphics& graphics, const D3D11_CULL_MODE cullMode);

	static std::string ResolveID(const D3D11_CULL_MODE cullMode) noexcept;
};
