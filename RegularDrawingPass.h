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
	RegularDrawingPass(Graphics& graphics);

	void Execute(Graphics& graphics, const std::vector<std::unique_ptr<Actor>>& actors, const Camera* const mainCamera);
};
