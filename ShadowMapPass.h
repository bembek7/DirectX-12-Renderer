#pragma once
#include "Pass.h"
#include "DepthStencilView.h"

class Actor;
class PointLight;
class DepthCubeTexture;
class Camera;
class Light;
class DirectionalLight;

class ShadowMapPass : public Pass
{
public:
	ShadowMapPass(Graphics& graphics);

	void Execute(Graphics& graphics, const std::vector<std::unique_ptr<Actor>>& actors, const std::vector<Light*>& lights, const DirectionalLight* const directionalLight);

private:
	std::unique_ptr<DepthStencilView> depthStencilView;
};
