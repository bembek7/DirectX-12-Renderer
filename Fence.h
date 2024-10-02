#pragma once
#include "d3dx12\d3dx12.h"
class Graphics;

class Fence
{
public:
	Fence(Graphics& graphics);
	void WaitForQueueFinish(Graphics& graphics, DWORD maxWaitingTime);

	void CloseEventHandle();

private:
	HANDLE fenceEvent;
	Microsoft::WRL::ComPtr<ID3D12Fence> fence;
	UINT64 fenceValue;
};
