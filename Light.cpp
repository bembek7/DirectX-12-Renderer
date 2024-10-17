#include "Light.h"
#include "Camera.h"
#include "RootParametersDescription.h"

Light::Light(Graphics& graphics, const std::string& fileName, const std::string& actorName) :
	MeshActor(graphics, fileName, actorName)
{
	shadowMapCamera = SceneComponent::AttachComponents<Camera>(std::move(Camera::CreateComponent("Shadow Map Camera")), rootComponent.get());

	constantBuffers.push_back(std::make_unique<ConstantBuffer<ShadowMapBuffer>>(graphics, shadowMapBuffer, RPD::ShadowMap));

	graphics.SetLight(this);
}

void Light::Bind(ID3D12GraphicsCommandList* const commandList)
{
	for (auto& cB : constantBuffers)
	{
		cB->Bind(commandList);
	}
}

void Light::Update(Graphics& graphics)
{
	Actor::Update(graphics);

	DirectX::XMStoreFloat4x4(&shadowMapBuffer.lightPerspective, DirectX::XMMatrixTranspose(shadowMapCamera->GetMatrix()));

	for (auto& cB : constantBuffers)
	{
		cB->Update();
	}
}

DirectX::XMMATRIX Light::GetLightPerspective() const noexcept
{
	return shadowMapCamera->GetMatrix();
}