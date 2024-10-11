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

	auto light = std::make_unique<PointLight>(window.GetGraphics(), meshesPath + "lightSphere.obj", "Point Light");
	auto brickWall = std::make_unique<MeshActor>(window.GetGraphics(), meshesPath + "brick_wall.obj", "Brick Wall");
	auto sphere = std::make_unique<MeshActor>(window.GetGraphics(), meshesPath + "sphere.obj", "Sphere1");

	//auto sponza = std::make_shared<MeshActor>(window.GetGraphics(), meshesPath + "sponza.obj", "Sponza");

	Dx::XMFLOAT3 zeroVec = { 0.f, 0.f, 0.f };
	brickWall->SetActorLocation(Dx::XMFLOAT3{ 0.f, 0.f, 2.5f });
	sphere->SetActorTransform({ 2.f, 0.f, 6.5f }, zeroVec, { 0.5f, 0.5f, 0.5f });
	light->SetActorScale(Dx::XMFLOAT3{ 0.2f, 0.2f, 0.2f });

	scene->AddActor(std::move(sphere));
	scene->AddActor(std::move(brickWall));
	scene->AddLight(std::move(light));
}

int App::Run()
{
	InitializeScene();
	auto& graphics = window.GetGraphics();
	auto const gui = graphics.GetGui();

	const std::string meshesPath = "Meshes\\";

	while (true)
	{
		if (const auto ecode = Window::ProcessMessages())
		{
			return *ecode;
		}

		HandleInput();

		scene->Draw(graphics);
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