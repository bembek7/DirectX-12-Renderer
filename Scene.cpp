#include "Scene.h"
#include "Light.h"
#include <stdexcept>
#include "Graphics.h"
#include "RegularDrawingPass.h"
//#include "DepthCubeTexture.h"

namespace Dx = DirectX;

Scene::Scene(Graphics& graphics)
{
	mainCamera = Camera::CreateComponent();
	mainCamera->AddRelativeLocation(Dx::XMVECTOR{ 0.0f, 0.0f, -6.0f });
	/*const float shadowMapCubeFaceSize = 1024.f;
	auto shadowMapCube = std::make_shared<DepthCubeTexture>(graphics, 0, (UINT)shadowMapCubeFaceSize);

	shadowMapPass = std::make_unique<ShadowMapPass>(graphics, shadowMapCube);*/
	drawingPass = std::make_unique<RegularDrawingPass>(graphics);
}

void Scene::AddActor(std::unique_ptr<Actor> actorToAdd)
{
	if (dynamic_cast<Light*>(actorToAdd.get()))
	{
		throw std::runtime_error("Light should be added using add light function");
	}
	actors.push_back(std::move(actorToAdd));
}

void Scene::AddLight(std::unique_ptr<Light> lightToAdd)
{
	if (light)
	{
		throw std::runtime_error("Only scenes with one light are handled right now");
	}
	light = lightToAdd.get();
	actors.push_back(std::move(lightToAdd));
}

//void Scene::AddSkybox(std::unique_ptr<Skybox> skyboxToAdd)
//{
//	if (skybox)
//	{
//		throw std::runtime_error("Only one skyubox should be added to scene");
//	}
//	skybox = std::move(skyboxToAdd);
//}

void Scene::Draw(Graphics& graphics)
{
	//shadowMapPass->Execute(graphics, actors, light.get());

	// Skybox should be drawn last
	//skybox->Draw(graphics);
	for (auto& actor : actors)
	{
		actor->Update(graphics);
	}
	drawingPass->Execute(graphics, actors, mainCamera.get());

	RenderControls(graphics);
}

void Scene::RenderControls(Graphics& graphics)
{
	for (auto& actor : actors)
	{
		graphics.GetGui()->RenderActorTree(actor.get());
	}
	graphics.GetGui()->RenderControlWindow();
}

Camera* Scene::GetMainCamera()
{
	return mainCamera.get();
}