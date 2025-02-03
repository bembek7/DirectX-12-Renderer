#pragma once
#include "Bindable.h"

class Graphics;

class PipelineState : public Bindable
{
public:
	struct PipelineStateStream
	{
		CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE rootSignature;
		CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT inputLayout;
		CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY primitiveTopologyType;
		CD3DX12_PIPELINE_STATE_STREAM_VS vertexShader;
		CD3DX12_PIPELINE_STATE_STREAM_PS pixelShader;
		CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS renderTargetFormats;
		CD3DX12_PIPELINE_STATE_STREAM_RASTERIZER rasterizer;
		CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT dsvFormat;
		CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL depthStencil;
		CD3DX12_PIPELINE_STATE_STREAM_BLEND_DESC blendDesc;
	};

	PipelineState(Graphics& graphics, PipelineStateStream& pipelineStateStream);
	void Bind(ID3D12GraphicsCommandList* const commandList) noexcept override;

private:
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;
};
