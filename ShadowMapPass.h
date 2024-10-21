#pragma once
#include "Pass.h"

class Actor;
class PointLight;
class DepthCubeTexture;

class ShadowMapPass : public Pass
{
public:
	ShadowMapPass(Graphics& graphics);

	virtual void Execute(Graphics& graphics, const std::vector<std::shared_ptr<Actor>>& actors, PointLight* const pointLight);

private:
	//std::shared_ptr<DepthCubeTexture> shadowMapCube;
};
