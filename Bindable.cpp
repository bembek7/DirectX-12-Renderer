#include "Bindable.h"
#include "Graphics.h"

ID3D12Device2* Bindable::GetDevice(Graphics& graphics) noexcept
{
	return graphics.device.Get();
}

ID3D12GraphicsCommandList* Bindable::GetCommandList(Graphics& graphics) noexcept
{
	return graphics.commandList.Get();
}

//std::vector<CD3DX12_ROOT_PARAMETER>& Bindable::GetRootParameters(Graphics& graphics) noexcept
//{
//	return graphics.rootParameters;
//}
//
//Graphics::PipelineStateStream& Bindable::GetPipelineStateStream(Graphics& graphics) noexcept
//{
//	return graphics.pipelineStateStream;
//}