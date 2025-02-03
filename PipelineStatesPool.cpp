#include "PipelineStatesPool.h"

PipelineStatesPool& PipelineStatesPool::GetInstance()
{
	static PipelineStatesPool instance;
	return instance;
}

std::shared_ptr<PipelineState> PipelineStatesPool::GetPipelineState(Graphics& graphics, const ShaderSettings shaderSettings, PipelineState::PipelineStateStream& pipelineStateStream)
{
	auto psIt = psMap.find(shaderSettings);
	if (psIt == psMap.end())
	{
		psMap[shaderSettings] = std::make_shared<PipelineState>(graphics, pipelineStateStream);
	}
	return psMap[shaderSettings];
}