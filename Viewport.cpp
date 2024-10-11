#include "Viewport.h"

Viewport::Viewport(const float width, const float height)
{
	viewport = CD3DX12_VIEWPORT{ 0.f, 0.f, width, height };
}

void Viewport::Bind(ID3D12GraphicsCommandList* const commandList) noexcept
{
	commandList->RSSetViewports(1, &viewport);
}