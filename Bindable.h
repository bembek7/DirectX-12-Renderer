#pragma once
#include <string>
#include "d3dx12\d3dx12.h"

class Graphics;

class Bindable
{
public:
	virtual void Bind(ID3D12GraphicsCommandList* const commandList) noexcept = 0;
	virtual ~Bindable() = default;

protected:
	static ID3D12Device2* GetDevice(Graphics& graphics) noexcept;
	static ID3D12GraphicsCommandList* GetCommandList(Graphics& graphics) noexcept;
	//static std::vector<CD3DX12_ROOT_PARAMETER>& GetRootParameters(Graphics& graphics) noexcept;
	//static Graphics::PipelineStateStream& GetPipelineStateStream(Graphics& graphics) noexcept;
};
