#pragma once
#include "Pass.h"
#include "PipelineState.h"
#include "DepthStencilView.h"

class Actor;
class Camera;

class DepthPrePass : public Pass
{
public:
	DepthPrePass(Graphics& graphics, const Camera* camera, DirectX::XMFLOAT4X4 projection);

	virtual void Execute(Graphics& graphics, const std::vector<std::unique_ptr<Actor>>& actors) override;
private:
	std::unique_ptr<DepthStencilView> depthStencilView;
};

