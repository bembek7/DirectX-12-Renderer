#include "Viewport.h"

Viewport::Viewport(const float width, const float height)
{
	viewport.Width = width;
	viewport.Height = height;
}

void Viewport::Bind(Graphics& graphics) noexcept
{
	GetCommandList(graphics)->RSSetViewports(1, &viewport);
}