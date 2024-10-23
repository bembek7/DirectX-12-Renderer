#pragma once
#include "Pass.h"
#include "DepthStencilView.h"
#include "PipelineState.h"

class Graphics;
class Camera;
class Actor;
class Light;
class DepthCubeTexture;

class RegularDrawingPass : public Pass
{
public:
	RegularDrawingPass(Graphics& graphics);

	void Execute(Graphics& graphics, const std::vector<std::unique_ptr<Actor>>& actors, const std::vector<Light*>& lights, const Camera* const mainCamera);
	static PipelineState::PipelineStateStream GetCommonPSS() noexcept;
private:
	std::unique_ptr<DepthStencilView> depthStencilView;
};
