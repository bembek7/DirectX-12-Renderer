#include "Viewport.h"

Viewport::Viewport(const float width, const float height)
{
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = width;
	viewport.Height = height;
	viewport.MaxDepth = 1.f;
	viewport.MinDepth = 0.f;
}

void Viewport::Bind(Graphics& graphics) noexcept
{
	GetContext(graphics)->RSSetViewports(1u, &viewport);
}