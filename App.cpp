#include "App.h"
#include <chrono>
#include "Mesh.h"
#include "PointLight.h"
#include <numbers>
#include "SoundPlayer.h"
#include "Camera.h"

int App::Run()
{
	Mesh plane = Mesh(window.GetGraphics(), "plane.obj", ShaderType::Phong, { 0.f, 0.f, 7.f }, { 0.f, 0.f, 0.f }, { 10.f, 10.f, 1.f });

	Mesh sphere = Mesh(window.GetGraphics(), "sphere.obj", ShaderType::Phong, { 0.f, 0.f, 6.f }, { 0.f, 0.f, 0.f }, { 0.5f, 0.5f, 0.5f });

	Mesh sphere2 = Mesh(window.GetGraphics(), "sphere.obj", ShaderType::Solid, { 0.f, 0.f, 4.5f }, { 0.f, 0.f, 0.f }, { 0.1f, 0.1f, 0.1f });
	Mesh sphere3 = Mesh(window.GetGraphics(), "sphere.obj", ShaderType::Phong, { -3.f, -3.f, 6.f }, { 0.f, 0.f, 0.f }, { 0.5f, 0.5f, 0.5f });
	PointLight pointLight = PointLight(window.GetGraphics(), { 0.f, 0.f, 4.5f });
	Camera camera;

	while (true)
	{
		if (const auto ecode = Window::ProcessMessages())
		{
			return *ecode;
		}

		while (!window.IsKeyQueueEmpty())
		{
			const short keyPressed = window.PopPressedKey();
		}

		if (window.IsKeyPressed('W'))
		{
			camera.Move({ 0.f,0.f,0.1f });
		}
		if (window.IsKeyPressed('S'))
		{
			camera.Move({ 0.f,0.f,-0.1f });
		}
		if (window.IsKeyPressed('D'))
		{
			camera.Move({ 0.1f,0.f,0.0f });
		}
		if (window.IsKeyPressed('A'))
		{
			camera.Move({ -0.1f,0.f,-0.1f });
		}
		if (window.IsKeyPressed(VK_LEFT))
		{
			camera.Rotate({ 0.0f,-0.01f,0.0f });
		}
		if (window.IsKeyPressed(VK_RIGHT))
		{
			camera.Rotate({ 0.0f,0.01f,0.0f });
		}
		if (window.IsKeyPressed(VK_UP))
		{
			camera.Rotate({ -0.01f,-0.0f,0.0f });
		}
		if (window.IsKeyPressed(VK_DOWN))
		{
			camera.Rotate({ 0.01f,0.0f,0.0f });
		}

		window.GetGraphics().SetCamera(camera.GetMatrix()); // Should be set every frame if doing camera movement
		window.GetGraphics().BeginFrame();
		pointLight.Bind(window.GetGraphics(), window.GetGraphics().GetCamera());
		sphere.Draw(window.GetGraphics());
		sphere2.Draw(window.GetGraphics());
		sphere3.Draw(window.GetGraphics());
		plane.Draw(window.GetGraphics());
		window.GetGraphics().EndFrame();
	}

	return 0;
}

App::HitType App::EvaluateHit(const double timeFromStart, const float timeStep) noexcept
{
	double missTime = std::fmod(timeFromStart, timeStep);
	if (missTime > timeStep / 2)
	{
		missTime = std::abs(timeStep - missTime);
	}

	if (missTime < 0.1f)
	{
		return HitType::Perfect;
	}
	else if (missTime < 0.15f)
	{
		return HitType::Good;
	}
	return HitType::Miss;
}