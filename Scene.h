#pragma once
#include <memory>
#include <vector>
#include "Camera.h"
#include "RegularDrawingPass.h"
//#include "ShadowMapPass.h"
//#include "Skybox.h"
#include "Actor.h"

class Graphics;
class PointLight;

class Scene
{
public:
	Scene(Graphics& graphics);

	void AddActor(std::unique_ptr<Actor> actorToAdd);
	void AddLight(std::unique_ptr<PointLight> lightToAdd);
	//void AddSkybox(std::unique_ptr<Skybox> skyboxToAdd);
	void Draw(Graphics& graphics);
	Camera* GetMainCamera();
private:
	void RenderControls(Graphics& graphics);

private:
	std::unique_ptr<RegularDrawingPass> drawingPass;
	/*std::unique_ptr<ShadowMapPass> shadowMapPass;*/
	std::vector<std::unique_ptr<Actor>> actors;
	PointLight* light = nullptr;
	//std::unique_ptr<Skybox> skybox;
	std::unique_ptr<Camera> mainCamera;
};
