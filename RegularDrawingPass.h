#pragma once
#include "Pass.h"

class Graphics;
class Camera;
class Actor;
class PointLight;
class DepthCubeTexture;

class RegularDrawingPass : public Pass
{
public:
	RegularDrawingPass(Graphics& graphics, std::shared_ptr<DepthCubeTexture> shadowMapCube);

	virtual void Execute(Graphics& graphics, const std::vector<std::shared_ptr<Actor>>& actors, PointLight* const pointLight, const Camera* const mainCamera);
};
