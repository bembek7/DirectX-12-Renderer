#include "App.h"
#include <chrono>
#include <numbers>
#include "MeshActor.h"
#include "PointLight.h"
#include <chrono>
#include "DirectionalLight.h"

namespace Dx = DirectX;

void App::InitializeScene()
{
	const std::string meshesPath = "Meshes\\";

	scene = std::make_unique<Scene>(window.GetGraphics());

	auto last = std::chrono::steady_clock::now();
	auto light = std::make_unique<DirectionalLight>(window.GetGraphics(), meshesPath + "lightSphere.obj");
	std::stringstream ss;
	ss << "Initalizing Light took: " << std::chrono::duration<float>(std::chrono::steady_clock::now() - last).count() << " seconds\n";
	OutputDebugString(ss.str().c_str());

	last = std::chrono::steady_clock::now();
	auto brickWall = std::make_unique<MeshActor>(window.GetGraphics(), meshesPath + "brick_wall.obj", "Brick Wall");
	ss = {};
	ss << "Initalizing Brick wall took: " << std::chrono::duration<float>(std::chrono::steady_clock::now() - last).count() << " seconds\n";
	OutputDebugString(ss.str().c_str());

	last = std::chrono::steady_clock::now();
	auto sphere = std::make_unique<MeshActor>(window.GetGraphics(), meshesPath + "sphere.obj", "Sphere1");
	ss = {};
	ss << "Initalizing sphere took: " << std::chrono::duration<float>(std::chrono::steady_clock::now() - last).count() << " seconds\n";
	OutputDebugString(ss.str().c_str());

	/*for (size_t i = 0; i < 100; i++)
	{
		auto brickWall2 = std::make_unique<MeshActor>(window.GetGraphics(), meshesPath + "brick_wall.obj", "Brick Wall");
		brickWall2->SetActorTransform(Dx::XMFLOAT3{ 5.f, 0.f, 2.5f + 0.5f * i }, Dx::XMFLOAT3{ 0.f, 0.f, 0.f + 10.5f * i }, Dx::XMFLOAT3{ 1.f, 1.f, 1.f });
		scene->AddActor(std::move(brickWall2));
	}*/
	last = std::chrono::steady_clock::now();
	auto sponza = std::make_unique<MeshActor>(window.GetGraphics(), meshesPath + "sponza.obj", "Sponza");
	ss = {};
	ss << "Initalizing sponza took: " << std::chrono::duration<float>(std::chrono::steady_clock::now() - last).count() << " seconds\n";
	OutputDebugString(ss.str().c_str());

	Dx::XMFLOAT3 zeroVec = { 0.f, 0.f, 0.f };

	sponza->SetActorTransform({ 0.f, -10.f, 0.0f }, zeroVec, Dx::XMFLOAT3{ 0.05f, 0.05f, 0.05f });
	//wall->SetActorLocation(Dx::XMFLOAT3{ 0.f, 0.f, 1.5f });
	brickWall->SetActorLocation(Dx::XMFLOAT3{ 0.f, 0.f, 2.5f });
	sphere->SetActorTransform({ 2.f, 0.f, 6.5f }, zeroVec, { 0.5f, 0.5f, 0.5f });
	light->SetActorScale(Dx::XMFLOAT3{ 0.2f, 0.2f, 0.2f });

	scene->AddActor(std::move(sponza));
	scene->AddActor(std::move(sphere));
	//scene->AddActor(std::move(wall));
	scene->AddActor(std::move(brickWall));
	scene->AddLight(std::move(light));
}

int App::Run()
{
	auto& graphics = window.GetGraphics();
	auto const gui = graphics.GetGui();

	auto last = std::chrono::steady_clock::now();
	InitializeScene();
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