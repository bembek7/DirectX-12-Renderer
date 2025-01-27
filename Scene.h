#pragma once
#include <memory>
#include <vector>
#include "Camera.h"
#include "GPass.h"
#include "FinalPass.h"
#include "Actor.h"
#include "LightPass.h"
#include "LightPerspectivePass.h"

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
	std::unique_ptr<GPass> gPass;
	std::unique_ptr<FinalPass> finalPass;
	std::vector<std::unique_ptr<Actor>> actors;
	std::vector<std::unique_ptr<LightPass>> lightPasses;
	std::vector<std::unique_ptr<LightPerspectivePass>> lightPerspectivePasses;
	std::unique_ptr<Camera> mainCamera;
	DirectX::XMFLOAT4X4 defaultProj;
	std::unique_ptr<RTVHeap> lightMapRtv;
};
