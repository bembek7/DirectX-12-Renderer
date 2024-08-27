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

	auto plane = std::make_shared<Mesh>(window.GetGraphics(), "plane.obj", ShaderType::Phong, nullptr, DirectX::XMFLOAT3{ 0.f, 0.f, 9.f }, DirectX::XMFLOAT3{ 10.f, 10.f, 1.f });
	auto sphere = std::make_shared<Mesh>(window.GetGraphics(), "sphere.obj", ShaderType::Phong, nullptr, DirectX::XMFLOAT3{ 0.f, 0.f, 6.5f }, DirectX::XMFLOAT3{ 0.5f, 0.5f, 0.5f });
	auto sphere2 = std::make_shared<Mesh>(window.GetGraphics(), "sphere.obj", ShaderType::Phong, nullptr, DirectX::XMFLOAT3{ -2.f, -2.f, 6.f }, DirectX::XMFLOAT3{ 0.5f, 0.5f, 0.5f });
	auto lightSphere = std::make_shared<Mesh>(window.GetGraphics(), "sphere.obj", ShaderType::Solid, nullptr, DirectX::XMFLOAT3{ 0.f, 1.f, 1.0f }, DirectX::XMFLOAT3{ 0.1f, 0.1f, 0.1f });

	meshes.push_back(plane);
	meshes.push_back(sphere);
	meshes.push_back(sphere2);
	meshes.push_back(lightSphere);

	auto pointLight = std::make_shared<PointLight>(window.GetGraphics(), lightSphere->GetComponentLocation());
	lights.push_back(pointLight);

	Camera camera = Camera(nullptr);

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

		DirectX::XMFLOAT2 cameraMoveInput = { 0.f, 0.f };
		DirectX::XMFLOAT2 cameraLookInput = { 0.f, 0.f };
		if (window.IsKeyPressed('W'))
		{
			cameraMoveInput.y += cameraMovementSpeed;
		}
		if (window.IsKeyPressed('S'))
		{
			cameraMoveInput.y -= cameraMovementSpeed;
		}
		if (window.IsKeyPressed('D'))
		{
			cameraMoveInput.x += cameraMovementSpeed;
		}
		if (window.IsKeyPressed('A'))
		{
			cameraMoveInput.x -= cameraMovementSpeed;
		}
		if (window.IsKeyPressed(VK_LEFT))
		{
			cameraLookInput.x -= cameraRotationSpeed;
		}
		if (window.IsKeyPressed(VK_RIGHT))
		{
			cameraLookInput.x += cameraRotationSpeed;
		}
		if (window.IsKeyPressed(VK_UP))
		{
			cameraLookInput.y -= cameraRotationSpeed;
		}
		if (window.IsKeyPressed(VK_DOWN))
		{
			cameraLookInput.y += cameraRotationSpeed;
		}
		camera.AddMovementInput(cameraMoveInput);
		camera.AddYawInput(cameraLookInput.x);
		camera.AddPitchInput(cameraLookInput.y);

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