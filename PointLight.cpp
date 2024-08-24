#include "PointLight.h"
#include "ThrowMacros.h"
#include "Graphics.h"

PointLight::PointLight(Graphics& graphics, const DirectX::XMVECTOR& position) :
	position(position)
{
	constantLightBuffer = std::make_unique<ConstantBuffer<LightBuffer>>(graphics, lightBuffer, BufferType::Pixel, 1u);
	shadowMapBuffer.lightPerspective = DirectX::XMMatrixTranspose(shadowMapCamera.GetMatrix());
	constantShadowMapBuffer = std::make_unique<ConstantBuffer<ShadowMapBuffer>>(graphics, shadowMapBuffer, BufferType::Vertex, 1u);
	shadowMapCamera.SetPosition(position);
}

void PointLight::SetDiffuseColor(Graphics& graphics, const DirectX::XMFLOAT3& newColor)
{
	lightBuffer.diffuseColor = newColor;
}

void PointLight::Bind(Graphics& graphics, const DirectX::XMMATRIX& cameraView)
{
	DirectX::XMStoreFloat3(&lightBuffer.lightViewPosition, DirectX::XMVector3Transform(position, cameraView));
	shadowMapBuffer.lightPerspective = DirectX::XMMatrixTranspose(shadowMapCamera.GetMatrix() * graphics.GetProjection());
	constantLightBuffer->Update(graphics);
	constantShadowMapBuffer->Update(graphics);
	constantLightBuffer->Bind(graphics);
	constantShadowMapBuffer->Bind(graphics);
}

DirectX::XMMATRIX PointLight::GetLightPerspective() const noexcept
{
	return shadowMapCamera.GetMatrix();
}
