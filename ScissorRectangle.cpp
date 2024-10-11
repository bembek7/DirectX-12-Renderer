#include "ScissorRectangle.h"

void ScissorRectangle::Bind(ID3D12GraphicsCommandList* const commandList) noexcept
{
	commandList->RSSetScissorRects(1, &scissorRect);
}