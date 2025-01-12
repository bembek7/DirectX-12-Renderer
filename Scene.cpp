#include "Scene.h"
#include "Light.h"
#include <stdexcept>
#include "Graphics.h"
#include "GPass.h"
#include "DirectionalLight.h"
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
	
	gPass = std::make_unique<GPass>(graphics, mainCamera.get(), defaultProj);
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
	actors.push_back(std::move(lightToAdd));
}

void Scene::Draw(Graphics& graphics)
{
	for (auto& actor : actors)
	{
		actor->Update(graphics);
	}

	gPass->Execute(graphics, actors);

	RenderControls(graphics);
}

void Scene::PrepareActorsForPasses(Graphics& graphics)
{

	for (const auto& actor : actors)
	{
		actor->PrepareForPass(graphics, gPass.get());
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