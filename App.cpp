#include "App.h"
#include <chrono>
#include "Mesh.h"
#include "PointLight.h"
#include <numbers>
#include "Camera.h"

int App::Run()
{
	Mesh plane = Mesh(window.GetGraphics(), "plane.obj", ShaderType::Phong, { 0.f, 0.f, 9.f }, { 0.f, 0.f, 0.f }, { 10.f, 10.f, 1.f });

	Mesh sphere = Mesh(window.GetGraphics(), "sphere.obj", ShaderType::Phong, { 0.f, 0.f, 6.5f }, { 0.f, 0.f, 0.f }, { 0.5f, 0.5f, 0.5f });

	Mesh lightSphere = Mesh(window.GetGraphics(), "sphere.obj", ShaderType::Solid, { 0.f, 1.f, 1.0f }, { 0.f, 0.f, 0.f }, { 0.1f, 0.1f, 0.1f });
	Mesh sphere3 = Mesh(window.GetGraphics(), "sphere.obj", ShaderType::Phong, { -2.f, -2.f, 6.f }, { 0.f, 0.f, 0.f }, { 0.5f, 0.5f, 0.5f });
	PointLight pointLight = PointLight(window.GetGraphics(), { 0.f, 1.f, 1.0f });
	Camera camera;

	constexpr float cameraMovementSpeed = 0.3f;
	constexpr float cameraRotationSpeed = 0.03f;

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
			camera.Move({ 0.f,0.f,cameraMovementSpeed });
		}
		if (window.IsKeyPressed('S'))
		{
			camera.Move({ 0.f,0.f,-cameraMovementSpeed });
		}
		if (window.IsKeyPressed('D'))
		{
			camera.Move({ cameraMovementSpeed,0.f,0.0f });
		}
		if (window.IsKeyPressed('A'))
		{
			camera.Move({ -cameraMovementSpeed,0.f,0.0f });
		}
		if (window.IsKeyPressed(VK_LEFT))
		{
			camera.Rotate({ 0.0f, -cameraRotationSpeed,0.0f });
		}
		if (window.IsKeyPressed(VK_RIGHT))
		{
			camera.Rotate({ 0.0f, cameraRotationSpeed, 0.0f });
		}
		if (window.IsKeyPressed(VK_UP))
		{
			camera.Rotate({ -cameraRotationSpeed, 0.0f, 0.0f });
		}
		if (window.IsKeyPressed(VK_DOWN))
		{
			camera.Rotate({ cameraRotationSpeed, 0.0f, 0.0f });
		}

		// Shadow Map rendering
		window.GetGraphics().BeginFrame();
		window.GetGraphics().SetRenderTargetForShadowMap();
		window.GetGraphics().SetCamera(pointLight.GetLightPerspective());
		sphere.RenderShadowMap(window.GetGraphics());
		sphere3.RenderShadowMap(window.GetGraphics());
		plane.RenderShadowMap(window.GetGraphics());

		// Regular drawing
		window.GetGraphics().SetNormalRenderTarget();
		window.GetGraphics().SetCamera(camera.GetMatrix());
		pointLight.Bind(window.GetGraphics(), window.GetGraphics().GetCamera());
		sphere.Draw(window.GetGraphics());
		lightSphere.Draw(window.GetGraphics());
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