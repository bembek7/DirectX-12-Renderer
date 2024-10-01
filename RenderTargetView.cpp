#include "RenderTargetView.h"
#include "ThrowMacros.h"
#include "Graphics.h"

RenderTargetView::RenderTargetView(Graphics& graphics)
{
	Microsoft::WRL::ComPtr<ID3D11Resource> backBuffer;
	CHECK_HR(GetSwapChain(graphics)->GetBuffer(0u, __uuidof(ID3D11Resource), &backBuffer));
	CHECK_HR(GetDevice(graphics)->CreateRenderTargetView(backBuffer.Get(), nullptr, &renderTargetView));
}

void RenderTargetView::Bind(Graphics& graphics) noexcept
{
	//graphics.SetCurrentRenderTargetView(renderTargetView);
}

void RenderTargetView::Update(Graphics& graphics)
{
	const float color[4] = { 0.04f, 0.02f, 0.1f, 1.0f };
	Clear(graphics, color);
}

void RenderTargetView::Clear(Graphics& graphics, const FLOAT color[4])
{
	GetContext(graphics)->ClearRenderTargetView(renderTargetView.Get(), color);
}

void NullRenderTargetView::Bind(Graphics& graphics) noexcept
{
	//graphics.SetCurrentRenderTargetView(nullptr);
}