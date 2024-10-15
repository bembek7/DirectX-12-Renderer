#include "PointLight.h"
#include "ThrowMacros.h"
#include "Graphics.h"
#include "Camera.h"
#include "MeshComponent.h"

PointLight::PointLight(Graphics& graphics, const std::string& fileName, const std::string& actorName) :
	MeshActor(graphics, fileName, actorName)
{
	shadowMapCamera = SceneComponent::AttachComponents<Camera>(std::move(Camera::CreateComponent("Shadow Map Camera")), rootComponent.get());

	auto& rootParams = graphics.GetCommonRootParametersRef();
	constantLightBuffer = std::make_unique<ConstantBuffer<LightBuffer>>(graphics, lightBuffer, 2u);

	constantShadowMapBuffer = std::make_unique<ConstantBuffer<ShadowMapBuffer>>(graphics, shadowMapBuffer, 1u);

	graphics.SetLight(this);
}

void PointLight::SetDiffuseColor(Graphics& graphics, const DirectX::XMFLOAT3 newColor)
{
	lightBuffer.diffuseColor = newColor;
}

void PointLight::Bind(Graphics& graphics, ID3D12GraphicsCommandList* const commandList)
{
	constantLightBuffer->Bind(commandList);
	constantShadowMapBuffer->Bind(commandList);
}

void PointLight::Update(Graphics& graphics)
{
	Actor::Update(graphics);
	DirectX::XMStoreFloat3(&lightBuffer.lightViewLocation, DirectX::XMVector3Transform(GetActorLocationVector(), graphics.GetCamera()));
	DirectX::XMStoreFloat4x4(&shadowMapBuffer.lightPerspective, DirectX::XMMatrixTranspose(shadowMapCamera->GetMatrix()));
	constantLightBuffer->Update();
	constantShadowMapBuffer->Update();
}

DirectX::XMMATRIX PointLight::GetLightPerspective() const noexcept
{
	return shadowMapCamera->GetMatrix();
}

void PointLight::RenderActorDetails(Gui& gui)
{
	Actor::RenderActorDetails(gui);
	gui.RenderActorDetails(this);
}