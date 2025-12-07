#include "App.h"
#include <chrono>
#include <numbers>
#include "MeshActor.h"
#include "PointLight.h"
#include "DirectionalLight.h"
#include "SpotLight.h"

namespace Dx = DirectX;

//#define BENCHMARK
#ifdef BENCHMARK
float gBenchmarkDeltaTime = 0.0f;
constexpr float benchmarkDuration = 20.0f;
#include <fstream>
#endif // BENCHMARK

void App::InitializeScene(Graphics& graphics)
{
	const std::string meshesPath = "Meshes\\";

	scene = std::make_unique<Scene>(window.GetGraphics());
	auto directionalLight = std::make_unique<DirectionalLight>(window.GetGraphics());
	//auto pointLight = std::make_unique<PointLight>(window.GetGraphics());
	//auto spotLight = std::make_unique<SpotLight>(window.GetGraphics());

	//auto sponza = std::make_unique<MeshActor>(window.GetGraphics(), meshesPath + "sponza.obj", "Sponza");
	auto sanMigeuel = std::make_unique<MeshActor>(window.GetGraphics(), meshesPath + "factory.obj", "SanMiguel");

	Dx::XMFLOAT3 zeroVec = { 0.f, 0.f, 0.f };

	//sponza->SetActorTransform({ 0.f, -10.f, 0.0f }, zeroVec, { 0.05f, 0.05f, 0.05f });
	//sanMigeuel->SetActorTransform({ 0.f, -10.f, 0.0f }, zeroVec, { 0.05f, 0.05f, 0.05f });

	//spotLight->SetActorLocation(Dx::XMFLOAT3{ 20.f, 0.f, 0.0f });
	//pointLight->SetActorLocation(Dx::XMFLOAT3{ 0.f, 0.f, -1.0f });

	//scene->AddActor(graphics, std::move(sponza));
	scene->AddActor(graphics, std::move(sanMigeuel));

	//scene->AddLight(graphics, std::move(pointLight));
	//scene->AddLight(graphics, std::move(spotLight));
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

#ifdef BENCHMARK
	// Benchmark: record ms per frame for 15 seconds
	std::vector<float> frameTimesMs;
	auto benchmarkStart = std::chrono::steady_clock::now();
#endif // BENCHMARK
	while (true)
	{
		const float deltaTime = std::chrono::duration<float>(std::chrono::steady_clock::now() - last).count();
		last = std::chrono::steady_clock::now();

#ifdef BENCHMARK
		gBenchmarkDeltaTime = deltaTime;
		// Record frame time if within benchmark duration
		if (std::chrono::duration<float>(last - benchmarkStart).count() < benchmarkDuration)
		{
			frameTimesMs.push_back(deltaTime * 1000.0f);
		}
		else if (!frameTimesMs.empty())
		{
			// Write results to file once after benchmark
			std::ofstream outFile("Benchmarking/benchmark.txt");
			for (const auto ms : frameTimesMs)
			{
				outFile << ms << '\n';
			}
			outFile.close();
			frameTimesMs.clear(); // Prevent repeated writes
		}
#endif // BENCHMARK

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
}

void App::HandleInput()
{
#ifdef BENCHMARK
	static auto benchmarkStart = std::chrono::steady_clock::now();
	float elapsed = std::chrono::duration<float>(std::chrono::steady_clock::now() - benchmarkStart).count();

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