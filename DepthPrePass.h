#pragma once
#include "Pass.h"
#include "PipelineState.h"
#include "DepthStencilView.h"

class Actor;
class Camera;

class DepthPrePass : public Pass
{
public:
	DepthPrePass(Graphics& graphics);

	void Execute(Graphics& graphics, const std::vector<std::unique_ptr<Actor>>& actors, const Camera* const mainCamera);
private:
	std::unique_ptr<DepthStencilView> depthStencilView;
};

