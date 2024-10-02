#include "Fence.h"
#include "ThrowMacros.h"
#include "Graphics.h"

Fence::Fence(Graphics& graphics)
{
	fenceValue = 0;
	CHECK_HR(graphics.device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));

	// fence signalling event
	HANDLE fenceEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
	if (!fenceEvent)
	{
		CHECK_HR(GetLastError());
		throw std::runtime_error{ "Failed to create fence event" };
	}
}

void Fence::WaitForQueueFinish(Graphics& graphics, DWORD maxWaitingTime)
{
	CHECK_HR(graphics.commandQueue->Signal(fence.Get(), ++fenceValue));
	CHECK_HR(fence->SetEventOnCompletion(fenceValue, fenceEvent));
	if (WaitForSingleObject(fenceEvent, maxWaitingTime) == WAIT_FAILED)
	{
		CHECK_HR(GetLastError());
	}
}

void Fence::CloseEventHandle()
{
	CloseHandle(fenceEvent);
}