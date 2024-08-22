#include "PointLight.h"
#include "ThrowMacros.h"
#include "Graphics.h"

PointLight::PointLight(Graphics& graphics, const DirectX::XMVECTOR& position) :
	position(position)
{
	constantLightBuffer = std::make_unique<ConstantBuffer<LightBuffer>>(graphics, lightBuffer, BufferType::Pixel, 1u);
}

void PointLight::SetDiffuseColor(Graphics& graphics, const DirectX::XMFLOAT3& newColor)
{
	lightBuffer.diffuseColor = newColor;
}

void PointLight::Bind(Graphics& graphics, const DirectX::XMMATRIX& cameraView)
{
	DirectX::XMStoreFloat3(&lightBuffer.lightViewPosition, DirectX::XMVector3Transform(position, cameraView));
	constantLightBuffer->Update(graphics);
	constantLightBuffer->Bind(graphics);
}