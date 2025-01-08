#pragma once
#include <memory>
#include <vector>
#include "Camera.h"
#include "RegularDrawingPass.h"
#include "LightPerspectivePass.h"
#include "DepthPrePass.h"
#include "Actor.h"

class Graphics;
class Light;

class Scene
{
public:
	Scene(Graphics& graphics);

	void AddActor(Graphics& graphics, std::unique_ptr<Actor> actorToAdd);
	void AddLight(Graphics& graphics, std::unique_ptr<Light> lightToAdd);
	void Draw(Graphics& graphics);
	void PrepareActorsForPasses(Graphics& graphics);
	Camera* GetMainCamera();
private:
	void RenderControls(Graphics& graphics);

private:
	std::vector<std::unique_ptr<Pass>> passes;
	std::vector<LightPerspectivePass*> lpPasses;
	std::vector<std::unique_ptr<Actor>> actors;
	std::vector<Light*> lights;
	DirectionalLight* directionalLight = nullptr;
	std::unique_ptr<Camera> mainCamera;
	DirectX::XMFLOAT4X4 defaultProj;
};
