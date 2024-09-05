#include "PointLight.h"
#include "ThrowMacros.h"
#include "Graphics.h"

PointLight::PointLight(Graphics& graphics, const std::string& fileName, const std::string& actorName) :
	MeshActor(graphics, fileName, actorName)
{
	shadowMapCamera = SceneComponent::AttachComponents<Camera>(std::move(Camera::CreateComponent("Shadow Map Camera")), rootComponent.get());

	constantLightBuffer = std::make_unique<ConstantBuffer<LightBuffer>>(graphics, lightBuffer, BufferType::Pixel, 0u);
	constantShadowMapBuffer = std::make_unique<ConstantBuffer<ShadowMapBuffer>>(graphics, shadowMapBuffer, BufferType::Vertex, 1u);
}

void PointLight::SetDiffuseColor(Graphics& graphics, const DirectX::XMFLOAT3 newColor)
{
	lightBuffer.diffuseColor = newColor;
}

void PointLight::Bind(Graphics& graphics)
{
	DirectX::XMStoreFloat3(&lightBuffer.lightViewLocation, DirectX::XMVector3Transform(GetActorLocationVector(), graphics.GetCamera()));
	DirectX::XMStoreFloat4x4(&shadowMapBuffer.lightPerspective, DirectX::XMMatrixTranspose(shadowMapCamera->GetMatrix() * graphics.GetProjection()));
	constantLightBuffer->Update(graphics);
	constantShadowMapBuffer->Update(graphics);
	constantLightBuffer->Bind(graphics);
	constantShadowMapBuffer->Bind(graphics);
}

DirectX::XMMATRIX PointLight::GetLightPerspective() const noexcept
{
	return shadowMapCamera->GetMatrix();
}