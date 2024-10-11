#include "PipelineState.h"
#include "d3dx12\d3dx12.h"
#include "ThrowMacros.h"
#include <d3dcompiler.h>
#include "Graphics.h"

namespace Wrl = Microsoft::WRL;

PipelineState::PipelineState(Graphics& graphics, PipelineStateStream& pipelineStateStream)
{
	const D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc = {
		sizeof(PipelineStateStream), &pipelineStateStream
	};
	CHECK_HR(GetDevice(graphics)->CreatePipelineState(&pipelineStateStreamDesc, IID_PPV_ARGS(&pipelineState)));
}

void PipelineState::Bind(ID3D12GraphicsCommandList* const commandList) noexcept
{
	commandList->SetPipelineState(pipelineState.Get());
}