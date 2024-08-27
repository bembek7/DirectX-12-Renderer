#include "PointLight.h"
#include "ThrowMacros.h"
#include "Graphics.h"

PointLight::PointLight(Graphics& graphics, const DirectX::XMFLOAT3 location)
{
	auto shadowMapCameraInit = std::make_unique<Camera>(nullptr);
	shadowMapCamera = shadowMapCameraInit.get();
	rootComponent = std::move(shadowMapCameraInit);

	constantLightBuffer = std::make_unique<ConstantBuffer<LightBuffer>>(graphics, lightBuffer, BufferType::Pixel, 1u);
	DirectX::XMStoreFloat4x4(&shadowMapBuffer.lightPerspective, DirectX::XMMatrixTranspose(shadowMapCamera->GetMatrix() * graphics.GetProjection()));
	constantShadowMapBuffer = std::make_unique<ConstantBuffer<ShadowMapBuffer>>(graphics, shadowMapBuffer, BufferType::Vertex, 1u);
	rootComponent->SetRelativeLocation(location);
}

void PointLight::SetDiffuseColor(Graphics& graphics, const DirectX::XMFLOAT3 newColor)
{
	lightBuffer.diffuseColor = newColor;
}

void PointLight::Bind(Graphics& graphics, const DirectX::XMMATRIX cameraView)
{
	DirectX::XMStoreFloat3(&lightBuffer.lightViewLocation, DirectX::XMVector3Transform(GetActorLocationVector(), cameraView));
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