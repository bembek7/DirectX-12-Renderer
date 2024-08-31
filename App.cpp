#include "App.h"
#include <chrono>
#include "MeshActor.h"
#include "PointLight.h"
#include <numbers>
#include "Camera.h"
#include "DirectXMath.h"

int App::Run()
{
	std::vector<std::shared_ptr<Actor>> allActors;
	std::vector<std::shared_ptr<MeshActor>> meshActors;
	std::vector<std::shared_ptr<PointLight>> lightActors;

	auto plane = std::make_shared<MeshActor>("Plane");
	auto sphere = std::make_shared<MeshActor>("Sphere");
	auto sphere2 = std::make_shared<MeshActor>("Sphere2");
	auto pointLight = std::make_shared<PointLight>(window.GetGraphics());

	plane->SetMesh(std::make_unique<Mesh>(window.GetGraphics(), "plane.obj", ShaderType::Phong));
	sphere->SetMesh(std::make_unique<Mesh>(window.GetGraphics(), "sphere.obj", ShaderType::Phong));
	sphere2->SetMesh(std::make_unique<Mesh>(window.GetGraphics(), "sphere.obj", ShaderType::Phong));
	pointLight->SetMesh(std::make_unique<Mesh>(window.GetGraphics(), "sphere.obj", ShaderType::Solid));

	DirectX::XMFLOAT3 zeroVec = { 0.f, 0.f, 0.f };
	plane->SetActorTransform({ 0.f, 0.f, 9.f }, zeroVec, { 10.f, 10.f, 1.f });
	sphere->SetActorTransform({ 2.f, 0.f, 6.5f }, zeroVec, { 0.5f, 0.5f, 0.5f });
	sphere2->SetActorTransform({ -2.f, -2.f, 6.f }, zeroVec, { 0.5f, 0.5f, 0.5f });
	pointLight->SetActorTransform({ 1.f, 1.f, 1.0f }, zeroVec, { 0.1f, 0.1f, 0.1f });

	meshActors.push_back(plane);
	meshActors.push_back(sphere);
	meshActors.push_back(sphere2);
	meshActors.push_back(pointLight);

	lightActors.push_back(pointLight);

	allActors.push_back(plane);
	allActors.push_back(sphere);
	allActors.push_back(sphere2);
	allActors.push_back(pointLight);

	auto camera = Camera::CreateComponent();

	while (true)
	{
		if (const auto ecode = Window::ProcessMessages())
		{
			return *ecode;
		}

		while (const auto keyPressed = window.ReadPressedKey())
		{
			if (keyPressed == VK_ESCAPE)
			{
				if (window.IsCursorEnabled())
				{
					window.DisableCursor();
					window.EnableRawInput();
				}
				else
				{
					window.EnableCursor();
					window.DisableRawInput();
				}
			}
		}

		DirectX::XMFLOAT2 cameraMoveInput = { 0.f, 0.f };
		DirectX::XMFLOAT2 cameraLookInput = { 0.f, 0.f };

		while (const auto rawDelta = window.ReadRawDelta())
		{
			cameraLookInput.x += rawDelta->first;
			cameraLookInput.y += rawDelta->second;
		}

		if (window.IsKeyPressed('W'))
		{
			cameraMoveInput.y += 1.f;
		}
		if (window.IsKeyPressed('S'))
		{
			cameraMoveInput.y -= 1.f;
		}
		if (window.IsKeyPressed('D'))
		{
			cameraMoveInput.x += 1.f;
		}
		if (window.IsKeyPressed('A'))
		{
			cameraMoveInput.x -= 1.f;
		}

		camera->AddMovementInput(cameraMoveInput);
		camera->AddYawInput(cameraLookInput.x);
		camera->AddPitchInput(cameraLookInput.y);

		window.GetGraphics().BeginFrame();

		// Shadow Map Rendering
		window.GetGraphics().SetRenderTargetForShadowMap();
		window.GetGraphics().SetCamera(pointLight->GetLightPerspective());
		for (auto& meshActor : meshActors)
		{
			meshActor->RenderShadowMap(window.GetGraphics());
		}

		// Regular drawing
		window.GetGraphics().SetNormalRenderTarget();
		window.GetGraphics().SetCamera(camera->GetMatrix());
		for (auto& lightActor : lightActors)
		{
			lightActor->Bind(window.GetGraphics(), window.GetGraphics().GetCamera());
		}
		for (auto& meshActor : meshActors)
		{
			meshActor->Draw(window.GetGraphics());
		}

		for (auto& actor : allActors)
		{
			window.GetGraphics().GetGui()->RenderActorControlWindow(actor.get());
		}

		window.GetGraphics().EndFrame();
	}

	return 0;
}