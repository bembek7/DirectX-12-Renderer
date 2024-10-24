#pragma once
#include "Pass.h"
#include "DepthStencilView.h"
#include "PipelineState.h"

class Actor;
class PointLight;
class DepthCubeTexture;
class Camera;
class Light;
class DirectionalLight;

class ShadowMappingPass : public Pass
{
public:
	ShadowMappingPass(Graphics& graphics);

	void Execute(Graphics& graphics, const std::vector<std::unique_ptr<Actor>>& actors, const std::vector<Light*>& lights, const DirectionalLight* const directionalLight);
	static PipelineState::PipelineStateStream GetCommonPSS() noexcept;
private:
	std::unique_ptr<DepthStencilView> depthStencilView;
};
