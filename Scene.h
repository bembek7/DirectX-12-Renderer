#pragma once
#include <memory>
#include <vector>
#include "Camera.h"
#include "RegularDrawingPass.h"
#include "ShadowMappingPass.h"
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
	void AddDirectionalLight(Graphics& graphics, std::unique_ptr<DirectionalLight> directionalLightToAdd);
	void Draw(Graphics& graphics);
	Camera* GetMainCamera();
private:
	void RenderControls(Graphics& graphics);

private:
	std::vector<std::unique_ptr<DepthPrePass>> passes;
	std::unique_ptr<RegularDrawingPass> drawingPass;
	std::unique_ptr<ShadowMappingPass> shadowMappingPass;
	std::vector<std::unique_ptr<Actor>> actors;
	std::vector<Light*> lights;
	DirectionalLight* directionalLight = nullptr;
	std::unique_ptr<Camera> mainCamera;
};
