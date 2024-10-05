#pragma once
#include <memory>
#include <vector>
#include "Camera.h"
//#include "RegularDrawingPass.h"
//#include "ShadowMapPass.h"
//#include "Skybox.h"

class Graphics;
class Actor;
class PointLight;

class Scene
{
public:
	Scene(Graphics& graphics);

	/*void AddActor(std::shared_ptr<Actor> actorToAdd);
	void AddLight(std::shared_ptr<PointLight> lightToAdd);
	void AddSkybox(std::unique_ptr<Skybox> skyboxToAdd);
	void Draw(Graphics& graphics);
	void RenderControls(Graphics& graphics);*/
	Camera* GetMainCamera();
private:
	/*std::unique_ptr<RegularDrawingPass> drawingPass;
	std::unique_ptr<ShadowMapPass> shadowMapPass;
	std::vector<std::shared_ptr<Actor>> actors;
	std::shared_ptr<PointLight> light;
	std::unique_ptr<Skybox> skybox;*/
	std::unique_ptr<Camera> mainCamera;
};
