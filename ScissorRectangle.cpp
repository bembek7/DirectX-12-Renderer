#include "ScissorRectangle.h"

void ScissorRectangle::Bind(Graphics& graphics) noexcept
{
	GetCommandList(graphics)->RSSetScissorRects(1, &scissorRect);
}