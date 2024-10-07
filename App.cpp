#include "App.h"
#include <chrono>
#include <numbers>
#include "MeshActor.h"

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
	auto sphere = std::make_shared<MeshActor>(window.GetGraphics(), meshesPath + "sphere.obj", "Sphere1");

	DirectX::XMFLOAT3 zeroVec = { 0.f, 0.f, 0.f };
	sphere->SetActorTransform({ 2.f, 0.f, 6.5f }, zeroVec, { 0.5f, 0.5f, 0.5f });

	float t = 0.f;
	constexpr float step = 0.01f;
	while (true)
	{
		if (const auto ecode = Window::ProcessMessages())
		{
			return *ecode;
		}

		HandleInput();
		window.GetGraphics().SetCamera(scene->GetMainCamera()->GetMatrix());
		window.GetGraphics().RenderBegin();
		sphere->Draw(window.GetGraphics());
		// drawing here
		window.GetGraphics().GetGui()->RenderActorTree(sphere.get());
		window.GetGraphics().GetGui()->RenderControlWindow();
		window.GetGraphics().RenderEnd();

		t += step;
	}

	window.GetGraphics().OnDestroy();

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