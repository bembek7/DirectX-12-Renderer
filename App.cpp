#include "App.h"
#include <chrono>
#include "MeshActor.h"
#include "PointLight.h"
#include <numbers>
#include "Camera.h"
#include "DirectXMath.h"
#include "Skybox.h"

int App::Run()
{
	std::vector<std::shared_ptr<Actor>> allActors;
	std::vector<std::shared_ptr<PointLight>> lightActors;

	const std::string meshesPath = "Meshes\\";
	auto skybox = Skybox(window.GetGraphics(), meshesPath + "skybox.obj");
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

	lightActors.push_back(pointLight);

	allActors.push_back(sphere);
	allActors.push_back(sphere2);
	allActors.push_back(pointLight);
	allActors.push_back(sponza);
	allActors.push_back(brickWall);

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

		window.GetGraphics().Draw(allActors, pointLight, camera.get());

		skybox.Draw(window.GetGraphics());

		for (auto& actor : allActors)
		{
			window.GetGraphics().GetGui()->RenderActorTree(actor.get());
		}
		window.GetGraphics().GetGui()->RenderControlWindow();

		window.GetGraphics().EndFrame();
	}

	return 0;
}