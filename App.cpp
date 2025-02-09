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

	//auto directionalLight = std::make_unique<DirectionalLight>(window.GetGraphics());
	auto pointLight = std::make_unique<PointLight>(window.GetGraphics());
	//auto spotLight = std::make_unique<SpotLight>(window.GetGraphics());
	auto sphere = std::make_unique<MeshActor>(window.GetGraphics(), meshesPath + "sphere.obj", "Sphere1");
	/*auto sphere2 = std::make_unique<MeshActor>(window.GetGraphics(), meshesPath + "sphere.obj", "Sphere2");
	auto sphere3 = std::make_unique<MeshActor>(window.GetGraphics(), meshesPath + "sphere.obj", "Sphere3");
	auto sphere4 = std::make_unique<MeshActor>(window.GetGraphics(), meshesPath + "sphere.obj", "Sphere4");
	auto left = std::make_unique<MeshActor>(window.GetGraphics(), meshesPath + "box.obj", "Left");
	auto right = std::make_unique<MeshActor>(window.GetGraphics(), meshesPath + "box.obj", "Right");
	auto front = std::make_unique<MeshActor>(window.GetGraphics(), meshesPath + "box.obj", "Front");
	auto back = std::make_unique<MeshActor>(window.GetGraphics(), meshesPath + "box.obj", "Back");*/
	/*sphere2->SetActorTransform({ 10.f, 0.f, 0.0f }, zeroVec, { 0.5f, 0.5f, 0.5f });
	sphere3->SetActorTransform({ 0.f, 0.f, -10.0f }, zeroVec, { 0.5f, 0.5f, 0.5f });
	sphere4->SetActorTransform({ 0.f, 0.f, 10.0f }, zeroVec, { 0.5f, 0.5f, 0.5f });
	left->SetActorTransform({ -15.f, 0.f, 0.0f }, zeroVec, { 0.1f, 30.5f, 30.5f });
	right->SetActorTransform({ 15.f, 0.f, 0.0f }, zeroVec, { 0.5f, 30.5f, 30.5f });
	front->SetActorTransform({ 0.f, 0.f, 15.0f }, zeroVec, { 30.5f, 30.5f, 0.5f });
	back->SetActorTransform({ 0.f, 0.f, -15.0f }, zeroVec, { 30.5f, 30.5f, 0.5f });*/
	/*scene->AddActor(graphics, std::move(sphere2));
	scene->AddActor(graphics, std::move(sphere3));
	scene->AddActor(graphics, std::move(sphere4));
	scene->AddActor(graphics, std::move(left));
	scene->AddActor(graphics, std::move(right));
	scene->AddActor(graphics, std::move(front));
	scene->AddActor(graphics, std::move(back));*/

	auto last = std::chrono::steady_clock::now();
	//auto sponza = std::make_unique<MeshActor>(window.GetGraphics(), meshesPath + "sponza.obj", "Sponza");
	std::stringstream ss = {};
	ss << "Initalizing sponza took: " << std::chrono::duration<float>(std::chrono::steady_clock::now() - last).count() << " seconds\n";
	OutputDebugString(ss.str().c_str());

	Dx::XMFLOAT3 zeroVec = { 0.f, 0.f, 0.f };

	//sponza->SetActorTransform({ 0.f, -10.f, 0.0f }, zeroVec, { 0.05f, 0.05f, 0.05f });
	sphere->SetActorTransform({ -10.f, 0.f, 0.0f }, zeroVec, { 0.5f, 0.5f, 0.5f });

	//scene->AddActor(graphics, std::move(sponza));
	scene->AddActor(graphics, std::move(sphere));
	
	scene->AddLight(graphics, std::move(pointLight));
	//scene->AddLight(graphics, std::move(spotLight));
	//scene->AddLight(graphics, std::move(directionalLight));

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