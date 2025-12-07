#include "App.h"
#include <chrono>
#include <numbers>
#include "MeshActor.h"
#include "PointLight.h"
#include "DirectionalLight.h"
#include "SpotLight.h"

namespace Dx = DirectX;

#define BENCHMARK
#ifdef BENCHMARK
float gBenchmarkDeltaTime = 0.0f;
constexpr float benchmarkDuration = 20.0f;
std::chrono::steady_clock::time_point sceneStartTime;
#include <fstream>
#endif // BENCHMARK

enum class App::SceneType
{
	Factory,
	Sponza,
	// Add two more scenes as needed, e.g. Hangar, Office
	Hangar,
	Office
};

void App::InitializeScene(Graphics& graphics, SceneType sceneType, int lightSetup)
{
	const std::string meshesPath = "Meshes\\";
	scene = std::make_unique<Scene>(window.GetGraphics());

	// Select mesh based on sceneType
	std::unique_ptr<MeshActor> meshActor;
	switch (sceneType)
	{
	case SceneType::Factory:
		meshActor = std::make_unique<MeshActor>(window.GetGraphics(), meshesPath + "factory.obj", "Factory");
		break;
	case SceneType::Sponza:
		meshActor = std::make_unique<MeshActor>(window.GetGraphics(), meshesPath + "sponza.obj", "Sponza");
		meshActor->SetActorTransform({ 0.f, -10.f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.05f, 0.05f, 0.05f });
		break;
	case SceneType::Hangar:
		meshActor = std::make_unique<MeshActor>(window.GetGraphics(), meshesPath + "factory.obj", "Hangar");
		break;
	case SceneType::Office:
		meshActor = std::make_unique<MeshActor>(window.GetGraphics(), meshesPath + "sponza.obj", "Office");
		break;
	}
	scene->AddActor(graphics, std::move(meshActor));

	// Always add directional light
	scene->AddLight(graphics, std::make_unique<DirectionalLight>(window.GetGraphics()));

	if (lightSetup >= 2)
	{
		scene->AddLight(graphics, std::make_unique<SpotLight>(window.GetGraphics()));
	}
	if (lightSetup == 3)
	{
		scene->AddLight(graphics, std::make_unique<PointLight>(window.GetGraphics()));
	}

	scene->PrepareActorsForPasses(graphics);
}

int App::Run()
{
	auto& graphics = window.GetGraphics();
	auto const gui = graphics.GetGui();

#ifdef BENCHMARK
	SceneType sceneTypes[4] = { SceneType::Factory, SceneType::Sponza, SceneType::Hangar, SceneType::Office };
	constexpr int numLightSetups = 3;

	for (int sceneIdx = 0; sceneIdx < 4; ++sceneIdx)
	{
		for (int lightSetup = 1; lightSetup <= numLightSetups; ++lightSetup)
		{
			InitializeScene(graphics, sceneTypes[sceneIdx], lightSetup);
			auto last = std::chrono::steady_clock::now();

			std::vector<float> frameTimesMs;
			auto benchmarkStart = std::chrono::steady_clock::now();

			sceneStartTime = std::chrono::steady_clock::now();
			while (true)
			{
				const float deltaTime = std::chrono::duration<float>(std::chrono::steady_clock::now() - last).count();
				last = std::chrono::steady_clock::now();

				gBenchmarkDeltaTime = deltaTime;
				if (std::chrono::duration<float>(last - benchmarkStart).count() < benchmarkDuration)
				{
					frameTimesMs.push_back(deltaTime * 1000.0f);
				}
				else if (!frameTimesMs.empty())
				{
					std::string filename = "Benchmarking/benchmark_scene" + std::to_string(sceneIdx + 1) +
						"_lights" + std::to_string(lightSetup) + ".txt";
					std::ofstream outFile(filename);
					for (const auto ms : frameTimesMs)
					{
						outFile << ms << '\n';
					}
					outFile.close();
					frameTimesMs.clear();
					break; // End benchmark for this scenario
				}

				if (const auto ecode = Window::ProcessMessages())
				{
					return *ecode;
				}

				HandleInput();
				graphics.RenderBegin();
				scene->Draw(graphics);
				gui->RenderPerformanceInfo(unsigned int(1.0f / deltaTime), deltaTime * 1000.0f);
				graphics.RenderEnd();
				scene->ProcessRemovals(graphics);
			}
		}
	}
	graphics.OnDestroy();
	return 0;
#else
	InitializeScene(graphics, App::SceneType::Sponza, 1);
	auto last = std::chrono::steady_clock::now();
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
		scene->ProcessRemovals(graphics);
	}

	graphics.OnDestroy();

	return 0;
#endif // BENCHMARK
}

void App::HandleInput()
{
#ifdef BENCHMARK
	float elapsed = std::chrono::duration<float>(std::chrono::steady_clock::now() - sceneStartTime).count();

	extern float gBenchmarkDeltaTime;

	if (elapsed < benchmarkDuration)
	{
		DirectX::XMFLOAT2 cameraMoveInput = { 0.f, 0.f };
		DirectX::XMFLOAT2 cameraLookInput = { 0.f, 0.f };

		// Use radians for rotation
		constexpr float turnRightDegreesPerSecond = 90.0f / 3.0f / 0.15f;   // 30 deg/sec / rotation speed
		constexpr float turnAroundDegreesPerSecond = 180.0f / 3.0f / 0.15f; // 60 deg/sec / rotation speed
		constexpr float moveSpeed = 15.0f; // units per second (increase for faster movement

		if (elapsed < 3.0f)
		{
			// Turn right 90° over 3 seconds
			cameraLookInput.x = turnRightDegreesPerSecond * gBenchmarkDeltaTime;
		}
		else if (elapsed < 6.0f)
		{
			// Move forward
			cameraMoveInput.y = moveSpeed * gBenchmarkDeltaTime;
		}
		else if (elapsed < 9.0f)
		{
			// Turn around 180° over 3 seconds
			cameraLookInput.x = -turnAroundDegreesPerSecond * gBenchmarkDeltaTime;
		}
		else if (elapsed < 17.0f)
		{
			// Move forward (opposite direction)
			cameraMoveInput.y = moveSpeed * gBenchmarkDeltaTime;
		}
		// Last 3 seconds: no movement

		scene->GetMainCamera()->AddMovementInput(cameraMoveInput);
		scene->GetMainCamera()->AddYawInput(cameraLookInput.x);
		scene->GetMainCamera()->AddPitchInput(cameraLookInput.y);
		return;
	}
#endif // BENCHMARK

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