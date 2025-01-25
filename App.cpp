#include "App.h"
#include <chrono>
#include <numbers>
#include "MeshActor.h"
#include "PointLight.h"
#include "DirectionalLight.h"
#include "SpotLight.h"

namespace Dx = DirectX;

void App::InitializeScene(Graphics& graphics)
{
	const std::string meshesPath = "Meshes\\";

	scene = std::make_unique<Scene>(window.GetGraphics());

	auto directionalLight = std::make_unique<DirectionalLight>(window.GetGraphics(), meshesPath + "lightSphere.obj");
	//auto pointLight = std::make_unique<PointLight>(window.GetGraphics(), meshesPath + "lightSphere.obj");
	// auto spotLight = std::make_unique<SpotLight>(window.GetGraphics(), meshesPath + "lightSphere.obj");

	auto brickWall = std::make_unique<MeshActor>(window.GetGraphics(), meshesPath + "brick_wall.obj", "Brick Wall");
	auto sphere = std::make_unique<MeshActor>(window.GetGraphics(), meshesPath + "sphere.obj", "Sphere1");

	auto last = std::chrono::steady_clock::now();
	auto sponza = std::make_unique<MeshActor>(window.GetGraphics(), meshesPath + "sponza.obj", "Sponza");
	std::stringstream ss = {};
	ss << "Initalizing sponza took: " << std::chrono::duration<float>(std::chrono::steady_clock::now() - last).count() << " seconds\n";
	OutputDebugString(ss.str().c_str());

	Dx::XMFLOAT3 zeroVec = { 0.f, 0.f, 0.f };

	sponza->SetActorTransform({ 0.f, -10.f, 0.0f }, zeroVec, { 0.05f, 0.05f, 0.05f });
	brickWall->SetActorLocation(Dx::XMFLOAT3{ 0.f, 0.f, 2.5f });
	sphere->SetActorTransform({ 2.f, 0.f, 6.5f }, zeroVec, { 0.5f, 0.5f, 0.5f });
	directionalLight->SetActorTransform({ 0.f, 80.f, 0.0f }, zeroVec, { 0.1f, 0.1f, 0.1f });
	//spotLight->SetActorTransform({ 30.f, 0.f, 0.0f }, zeroVec, { 0.1f, 0.1f, 0.1f });
	//pointLight->SetActorTransform({ -10.f, 0.f, 0.0f }, zeroVec, Dx::XMFLOAT3{ 0.1f, 0.1f, 0.1f });

	scene->AddActor(graphics, std::move(sponza));
	scene->AddActor(graphics, std::move(sphere));
	scene->AddActor(graphics, std::move(brickWall));
	// scene->AddLight(graphics, std::move(pointLight));
	// scene->AddLight(graphics, std::move(spotLight));
	scene->AddLight(graphics, std::move(directionalLight));

	scene->PrepareActorsForPasses(graphics);
}

int App::Run()
{
	auto& graphics = window.GetGraphics();
	auto const gui = graphics.GetGui();

	auto last = std::chrono::steady_clock::now();
	InitializeScene(graphics);
	std::stringstream ss;
	ss << "Scene initialization took: " << std::chrono::duration<float>(std::chrono::steady_clock::now() - last).count() << " seconds";
	OutputDebugString(ss.str().c_str());
	last = std::chrono::steady_clock::now();

	while (true)
	{
		const float deltaTime = std::chrono::duration<float>(std::chrono::steady_clock::now() - last).count();
		last = std::chrono::steady_clock::now();

		if (const auto ecode = Window::ProcessMessages())
		{
			return *ecode;
		}

		HandleInput();
		graphics.RenderBegin();
		scene->Draw(graphics);
		gui->RenderPerformanceInfo(unsigned int(1.0f / deltaTime), deltaTime * 1000.0f);
		graphics.RenderEnd();
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