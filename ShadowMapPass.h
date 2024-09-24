#pragma once
#include "Pass.h"

class Actor;
class PointLight;
class DepthCubeTexture;

class ShadowMapPass : public Pass
{
public:
	ShadowMapPass(Graphics& graphics, const float windowWidth, const float windowHeight, std::shared_ptr<DepthCubeTexture> shadowMapCubeTex);

	virtual void Execute(Graphics& graphics, const std::vector<std::shared_ptr<Actor>>& actors, const std::shared_ptr<PointLight>& pointLight);

private:
	std::shared_ptr<DepthCubeTexture> shadowMapCube;
};
