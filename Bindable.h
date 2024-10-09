#pragma once
#include <string>
#include "d3dx12\d3dx12.h"

class Graphics;

class Bindable
{
public:
	virtual void Bind(Graphics& graphics) noexcept = 0;
	virtual void Update(Graphics& graphics);
	virtual ~Bindable() = default;
	void WaitForQueueFinish(Graphics& graphics);

protected:
	static ID3D12Device2* GetDevice(Graphics& graphics) noexcept;
	static ID3D12GraphicsCommandList* GetCommandList(Graphics& graphics) noexcept;
	static ID3D12DescriptorHeap* GetSrvHeap(Graphics& graphics) noexcept;
	//static std::vector<CD3DX12_ROOT_PARAMETER>& GetRootParameters(Graphics& graphics) noexcept;
	//static Graphics::PipelineStateStream& GetPipelineStateStream(Graphics& graphics) noexcept;
};
