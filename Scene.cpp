#include "Scene.h"
#include "PointLight.h"
#include <stdexcept>
#include "Graphics.h"
#include "GPass.h"
#include "DirectionalLight.h"
#include "LightPerspectivePass.h"

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

	std::array<D3D12_RENDER_TARGET_VIEW_DESC, 1> lightMapRtvDescs =
	{
		D3D12_RENDER_TARGET_VIEW_DESC{ DXGI_FORMAT_R11G11B10_FLOAT, D3D12_RTV_DIMENSION_TEXTURE2D }
	};

	lightMapRtv = std::make_unique<RTVHeap>(graphics, 1, lightMapRtvDescs.data());

	gPass = std::make_unique<GPass>(graphics, mainCamera.get(), defaultProj);
	finalPass = std::make_unique<FinalPass>(graphics, gPass->GetColorTexture(), lightMapRtv->GetRenderTarget(0u));
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
	auto lpp = std::make_unique<LightPerspectivePass>(graphics, lightToAdd->GetLightCamera(), lightToAdd->GetLightProjection());
	
	auto lightPass = std::make_unique<LightPass>(graphics, gPass->GetNormal_RoughnessTexture(), gPass->GetSpecularColorTexture(),
												gPass->GetViewPositionTexture(), gPass->GetWorldPositionTexture(), lpp->GetDepthBuffer(),
												lightToAdd.get());
	lightPerspectivePasses.push_back(std::move(lpp));
	lightPasses.push_back(std::move(lightPass));
	actors.push_back(std::move(lightToAdd));
}

void Scene::Draw(Graphics& graphics)
{
	graphics.SetCamera(mainCamera->GetMatrix());
	graphics.SetProjection(Dx::XMLoadFloat4x4(&defaultProj));
	for (auto& actor : actors)
	{
		actor->Update(graphics);
	}

	gPass->Execute(graphics, actors);

	const float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	graphics.GetMainCommandList()->ClearRenderTargetView(lightMapRtv->GetCPUHandle(), clearColor, 0, nullptr);

	if (lightPerspectivePasses.size() != lightPasses.size())
	{
		throw std::runtime_error("Every light pass needs respective light perspective pass");
	}
	for (size_t i = 0; i < lightPerspectivePasses.size(); ++i)
	{
		lightPerspectivePasses[i]->Execute(graphics, actors);
		lightPasses[i]->Execute(graphics, lightMapRtv->GetCPUHandle());
	}
	finalPass->Execute(graphics);

	RenderControls(graphics);
}

void Scene::PrepareActorsForPasses(Graphics& graphics)
{
	for (const auto& actor : actors)
	{
		actor->PrepareForPass(graphics, gPass.get());
		for(auto& lpp : lightPerspectivePasses)
		{
			actor->PrepareForPass(graphics, lpp.get());
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