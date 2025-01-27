#include "Scene.h"
#include "PointLight.h"
#include <stdexcept>
#include "Graphics.h"
#include "GPass.h"
#include "DirectionalLight.h"
#include "ShadowPass.h"

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

	std::array<D3D12_RENDER_TARGET_VIEW_DESC, 1> shadowMapRtvDescs =
	{
		D3D12_RENDER_TARGET_VIEW_DESC{ DXGI_FORMAT_R32_FLOAT, D3D12_RTV_DIMENSION_TEXTURE2D }
	};

	shadowMapRtv = std::make_unique<RTVHeap>(graphics, 1, shadowMapRtvDescs.data());

	gPass = std::make_unique<GPass>(graphics, mainCamera.get(), defaultProj);
	finalPass = std::make_unique<FinalPass>(graphics, gPass->GetColorTexture(), lightMapRtv->GetRenderTarget(0));
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
	auto lightPass = std::make_unique<LightPass>(graphics, gPass->GetNormal_RoughnessTexture(), gPass->GetSpecularColorTexture(), gPass->GetViewPositionTexture(), lightToAdd.get());
	lightPasses.push_back(std::move(lightPass));
	//auto shadowPass = std::make_unique<ShadowPass>(graphics, lightToAdd->GetLightCamera(), defaultProj, gPass->GetDepthBuffer());
	//shadowPasses.push_back(std::move(shadowPass));
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
	graphics.GetMainCommandList()->ClearRenderTargetView(shadowMapRtv->GetCPUHandle(), clearColor, 0, nullptr);
	for(auto& lightPass : lightPasses)
	{
		lightPass->Execute(graphics, lightMapRtv->GetCPUHandle());
	}
	for (auto& shadowPass : shadowPasses)
	{
		shadowPass->Execute(graphics, actors, shadowMapRtv->GetCPUHandle());
	}
	finalPass->Execute(graphics);

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