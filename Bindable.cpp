#include "Bindable.h"
#include "Graphics.h"

void Bindable::Update(Graphics& graphics)
{}

ID3D11DeviceContext* Bindable::GetContext(Graphics& graphics) noexcept
{
	return graphics.context.Get();
}

ID3D11Device* Bindable::GetDevice(Graphics& graphics) noexcept
{
	return graphics.device.Get();
}

IDXGISwapChain* Bindable::GetSwapChain(Graphics& graphics) noexcept
{
	return graphics.swapChain.Get();
}