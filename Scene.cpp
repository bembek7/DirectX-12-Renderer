#include "Scene.h"
#include "Light.h"
#include <stdexcept>
#include "Graphics.h"
#include "RegularDrawingPass.h"
//#include "DepthCubeTexture.h"
#include "DirectionalLight.h"
#include "DepthPrePass.h"

namespace Dx = DirectX;

Scene::Scene(Graphics& graphics)
{
	mainCamera = Camera::CreateComponent();
	mainCamera->AddRelativeLocation(Dx::XMVECTOR{ 0.0f, 0.0f, -6.0f });
	/*const float ShadowMappingCubeFaceSize = 1024.f;
	auto ShadowMappingCube = std::make_shared<DepthCubeTexture>(graphics, 0, (UINT)ShadowMappingCubeFaceSize);*/
	graphics.SetCamera(mainCamera->GetMatrix());
	shadowMappingPass = std::make_unique<ShadowMappingPass>(graphics);
	drawingPass = std::make_unique<RegularDrawingPass>(graphics);
	passes.push_back(std::make_unique<DepthPrePass>(graphics));
}

void Scene::AddActor(Graphics& graphics, std::unique_ptr<Actor> actorToAdd)
{
	if (dynamic_cast<Light*>(actorToAdd.get()))
	{
		throw std::runtime_error("Light should be added using add light function");
	}
	for (const auto& pass : passes)
	{
		actorToAdd->PrepareForPass(graphics, pass.get());
	}
	actors.push_back(std::move(actorToAdd));
}

void Scene::AddLight(Graphics& graphics, std::unique_ptr<Light> lightToAdd)
{
	lights.push_back(lightToAdd.get());
	for (const auto& pass : passes)
	{
		lightToAdd->PrepareForPass(graphics, pass.get());
	}
	actors.push_back(std::move(lightToAdd));
}

void Scene::AddDirectionalLight(Graphics& graphics, std::unique_ptr<DirectionalLight> directionalLightToAdd)
{
	if (directionalLight)
	{
		throw std::runtime_error("There can be only one directional light in a scene");
	}
	directionalLight = directionalLightToAdd.get();
	AddLight(graphics, std::move(directionalLightToAdd));
}

void Scene::Draw(Graphics& graphics)
{
	for (auto& actor : actors)
	{
		actor->Update(graphics);
	}

	for (const auto& pass : passes)
	{
		pass->Execute(graphics, actors, mainCamera.get());
	}
	shadowMappingPass->Execute(graphics, actors, lights, directionalLight);
	drawingPass->Execute(graphics, actors, lights, mainCamera.get());

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