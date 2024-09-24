#include "App.h"
#include <chrono>
#include "MeshActor.h"
#include "PointLight.h"
#include <numbers>
#include "Camera.h"
#include "DirectXMath.h"
#include "Skybox.h"

void App::InitializeScene()
{
	const std::string meshesPath = "Meshes\\";
	auto brickWall = std::make_shared<MeshActor>(window.GetGraphics(), meshesPath + "brick_wall.obj", "BrickWall");
	auto sponza = std::make_shared<MeshActor>(window.GetGraphics(), meshesPath + "sponza.obj", "Sponza");
	sponza->SetActorScale(DirectX::XMFLOAT3{ 0.1f, 0.1f, 0.1f });

	auto sphere = std::make_shared<MeshActor>(window.GetGraphics(), meshesPath + "sphere.obj", "Sphere1");
	auto sphere2 = std::make_shared<MeshActor>(window.GetGraphics(), meshesPath + "sphere.obj", "Sphere2");

	auto pointLight = std::make_shared<PointLight>(window.GetGraphics(), meshesPath + "lightSphere.obj", "Point Light");

	DirectX::XMFLOAT3 zeroVec = { 0.f, 0.f, 0.f };
	sphere->SetActorTransform({ 2.f, 0.f, 6.5f }, zeroVec, { 0.5f, 0.5f, 0.5f });
	sphere2->SetActorTransform({ -5.f, 13.f, 21.0f }, zeroVec, { 0.5f, 0.5f, 0.5f });
	pointLight->SetActorTransform({ -5.f, 13.f, 13.0f }, zeroVec, { 0.1f, 0.1f, 0.1f });
	brickWall->SetActorTransform(DirectX::XMFLOAT3{ 5.f, 5.f, 5.f }, zeroVec, { 2.f, 2.f, 2.f });

	scene = std::make_unique<Scene>(window.GetGraphics());
	scene->AddLight(std::move(pointLight));
	scene->AddActor(std::move(sphere));
	scene->AddActor(std::move(sphere2));
	scene->AddActor(std::move(sponza));
	scene->AddActor(std::move(brickWall));
	scene->AddSkybox(std::make_unique<Skybox>(window.GetGraphics(), meshesPath + "skybox.obj"));
}

int App::Run()
{
	InitializeScene();

	while (true)
	{
		if (const auto ecode = Window::ProcessMessages())
		{
			return *ecode;
		}

		HandleInput();

		window.GetGraphics().BeginFrame();

		scene->Draw(window.GetGraphics());

		scene->RenderControls(window.GetGraphics());

		window.GetGraphics().EndFrame();
	}

	return 0;
}

void App::HandleInput()
{
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

	scene->GetMainCamera()->AddMovementInput(cameraMoveInput);
	scene->GetMainCamera()->AddYawInput(cameraLookInput.x);
	scene->GetMainCamera()->AddPitchInput(cameraLookInput.y);
}