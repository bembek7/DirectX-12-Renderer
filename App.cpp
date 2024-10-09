#include "App.h"
#include <chrono>
#include <numbers>
#include "MeshActor.h"
#include "PointLight.h"

namespace Dx = DirectX;

void App::InitializeScene()
{
	const std::string meshesPath = "Meshes\\";

	scene = std::make_unique<Scene>(window.GetGraphics());

	//scene->AddActor(std::move(sphere));
}

int App::Run()
{
	InitializeScene();

	const std::string meshesPath = "Meshes\\";
	auto light = std::make_shared<PointLight>(window.GetGraphics(), meshesPath + "lightSphere.obj", "Point Light");
	auto brickWall = std::make_shared<MeshActor>(window.GetGraphics(), meshesPath + "brick_wall.obj", "Brick Wall");
	auto sphere = std::make_shared<MeshActor>(window.GetGraphics(), meshesPath + "sphere.obj", "Sphere1");

	Dx::XMFLOAT3 zeroVec = { 0.f, 0.f, 0.f };
	brickWall->SetActorLocation(Dx::XMFLOAT3{ 0.f, 0.f, 2.5f });
	sphere->SetActorTransform({ 2.f, 0.f, 6.5f }, zeroVec, { 0.5f, 0.5f, 0.5f });
	light->SetActorScale(Dx::XMFLOAT3{ 0.2f, 0.2f, 0.2f });

	float t = 0.f;
	constexpr float step = 0.01f;

	auto& graphics = window.GetGraphics();
	auto const gui = graphics.GetGui();

	while (true)
	{
		if (const auto ecode = Window::ProcessMessages())
		{
			return *ecode;
		}

		HandleInput();
		graphics.SetCamera(scene->GetMainCamera()->GetMatrix());
		graphics.RenderBegin();

		light->Draw(graphics);
		graphics.SetLight(light.get());
		sphere->Draw(graphics);
		brickWall->Draw(graphics);
		// drawing here

		gui->RenderActorTree(sphere.get());
		gui->RenderActorTree(light.get());
		gui->RenderActorTree(brickWall.get());
		gui->RenderControlWindow();
		graphics.RenderEnd();

		t += step;
	}

	graphics.OnDestroy();

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