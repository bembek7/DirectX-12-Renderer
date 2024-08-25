#include "App.h"
#include <chrono>
#include "Mesh.h"
#include "PointLight.h"
#include <numbers>
#include "Camera.h"

int App::Run()
{
	std::vector<std::shared_ptr<Mesh>> meshes;
	std::vector<std::shared_ptr<PointLight>> lights;

	auto plane = std::make_shared<Mesh>(window.GetGraphics(), "plane.obj", ShaderType::Phong, DirectX::XMVECTOR{ 0.f, 0.f, 9.f }, DirectX::XMVECTOR{ 10.f, 10.f, 1.f });
	auto sphere = std::make_shared<Mesh>(window.GetGraphics(), "sphere.obj", ShaderType::Phong, DirectX::XMVECTOR{ 0.f, 0.f, 6.5f }, DirectX::XMVECTOR{ 0.5f, 0.5f, 0.5f });
	auto sphere2 = std::make_shared<Mesh>(window.GetGraphics(), "sphere.obj", ShaderType::Phong, DirectX::XMVECTOR{ -2.f, -2.f, 6.f }, DirectX::XMVECTOR{ 0.5f, 0.5f, 0.5f });
	auto lightSphere = std::make_shared<Mesh>(window.GetGraphics(), "sphere.obj", ShaderType::Solid, DirectX::XMVECTOR{ 0.f, 1.f, 1.0f }, DirectX::XMVECTOR{0.1f, 0.1f, 0.1f });
	
	meshes.push_back(plane);
	meshes.push_back(sphere);
	meshes.push_back(sphere2);
	meshes.push_back(lightSphere);

	auto pointLight = std::make_shared<PointLight>(window.GetGraphics(), lightSphere->GetLocation());
	lights.push_back(pointLight);

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

		
		window.GetGraphics().BeginFrame();

		// Shadow Map rendering
		window.GetGraphics().SetRenderTargetForShadowMap();
		window.GetGraphics().SetCamera(pointLight->GetLightPerspective());
		for (const auto& mesh : meshes)
		{
			mesh->RenderShadowMap(window.GetGraphics());
		}

		// Regular drawing
		window.GetGraphics().SetNormalRenderTarget();
		window.GetGraphics().SetCamera(camera.GetMatrix());
		for (const auto& light : lights)
		{
			light->Bind(window.GetGraphics(), window.GetGraphics().GetCamera());
		}
		for (const auto& mesh : meshes)
		{
			mesh->Draw(window.GetGraphics());
		}

		window.GetGraphics().EndFrame();
	}

	return 0;
}
