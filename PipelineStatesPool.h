#pragma once
#include <wrl\client.h>
#include <d3d12.h>
#include <string>
#include <memory>
#include <unordered_map>
#include "PipelineState.h"
#include "ShaderSettings.h"

class PipelineStatesPool
{
public:
	static PipelineStatesPool& GetInstance();
	PipelineStatesPool(PipelineStatesPool const&) = delete;
	void operator=(PipelineStatesPool const&) = delete;

	std::shared_ptr<PipelineState> GetPipelineState(Graphics& graphics, const ShaderSettings, PipelineState::PipelineStateStream& pipelineStateStream);
private:
	PipelineStatesPool() = default;
private:
	std::unordered_map<ShaderSettings, std::shared_ptr<PipelineState>, ShaderSettingsHash> psMap;
};
