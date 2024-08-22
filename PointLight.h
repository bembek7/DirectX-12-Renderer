#pragma once
#include <DirectXMath.h>
#include "ConstantBuffer.h"

class Graphics;

class PointLight
{
public:
	PointLight(Graphics& graphics, const DirectX::XMVECTOR& position);
	void SetDiffuseColor(Graphics& graphics, const DirectX::XMFLOAT3& newColor);
	void Bind(Graphics& graphics, const DirectX::XMMATRIX& cameraView);

private:
	struct LightBuffer
	{
		DirectX::XMFLOAT3 diffuseColor = { 1.0f, 1.0f, 1.0f };
		float diffuseIntensity = 0.6f;
		DirectX::XMFLOAT3 ambient = { 0.15f, 0.15f, 0.15f };
		float specularIntensity = 0.6f;
		DirectX::XMFLOAT3 lightViewPosition = { 0.f, 0.f, 0.f };
		float specularPower = 0.8f;
	} lightBuffer;

	DirectX::XMVECTOR position;

	std::unique_ptr<ConstantBuffer<LightBuffer>> constantLightBuffer;
};
