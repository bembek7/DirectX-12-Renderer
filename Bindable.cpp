#include "Bindable.h"
#include "Graphics.h"

void Bindable::Update(Graphics& graphics)
{}

void Bindable::WaitForQueueFinish(Graphics& graphics)
{
	graphics.fence->WaitForQueueFinish(graphics, INFINITE);
}

ID3D12Device2* Bindable::GetDevice(Graphics& graphics) noexcept
{
	return graphics.device.Get();
}

ID3D12GraphicsCommandList* Bindable::GetCommandList(Graphics& graphics) noexcept
{
	return graphics.commandList.Get();
}