#include "Scene.h"
#include "Light.h"
#include <stdexcept>
#include "Graphics.h"
#include "RegularDrawingPass.h"
#include "DirectionalLight.h"
#include "DepthPrePass.h"
#include "ShadowMapGenerationPass.h"

namespace Dx = DirectX;

Scene::Scene(Graphics& graphics)
{
	mainCamera = Camera::CreateComponent();
	mainCamera->AddRelativeLocation(Dx::XMVECTOR{ 0.0f, 0.0f, -6.0f });

	DirectX::XMMATRIX reverseZ =
	{
		1.0f, 0.0f,  0.0f, 0.0f,
		0.0f, 1.0f,  0.0f, 0.0f,
		0.0f, 0.0f, -1.0f, 0.0f,
		0.0f, 0.0f,  1.0f, 1.0f
	};
	const float windowWidth = graphics.GetWindowWidth();
	const float windowHeight = graphics.GetWindowHeight();

	Dx::XMStoreFloat4x4(&defaultProj, Dx::XMMatrixPerspectiveLH(1.0f, windowHeight / windowWidth, 0.5f, 200.0f) * reverseZ);
	
	passes.push_back(std::make_unique<DepthPrePass>(graphics, mainCamera.get(), defaultProj));
	//passes.push_back(std::make_unique<RegularDrawingPass>(graphics, mainCamera.get(), defaultProj));
}

void Scene::AddActor(Graphics& graphics, std::unique_ptr<Actor> actorToAdd)
{
	if (dynamic_cast<Light*>(actorToAdd.get()))
	{
		throw std::runtime_error("Light should be added using add light function");
	}
	actors.push_back(std::move(actorToAdd));
}

void Scene::AddLight(Graphics& graphics, std::unique_ptr<Light> lightToAdd)
{
	lights.push_back(lightToAdd.get());
	auto lpPass = std::make_unique<LightPerspectivePass>(graphics, lightToAdd->GetLightCamera(), lightToAdd->GetLightProjection());
	lpPasses.push_back(lpPass.get());
	passes.push_back(std::move(lpPass));
	actors.push_back(std::move(lightToAdd));
}

void Scene::Draw(Graphics& graphics)
{
	for (auto& actor : actors)
	{
		actor->Update(graphics);
	}

	for (const auto& pass : passes)
	{
		pass->Execute(graphics, actors);
	}

	RenderControls(graphics);
}

void Scene::PrepareActorsForPasses(Graphics& graphics)
{
	for (const auto& lpPass : lpPasses)
	{
		passes.push_back(std::make_unique<ShadowMapGenerationPass>(graphics, mainCamera.get(), defaultProj, lpPass));
	}

	for (const auto& actor : actors)
	{
		for (const auto& pass : passes)
		{
			actor->PrepareForPass(graphics, pass.get());
		}
	}
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