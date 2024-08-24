#pragma once
#include <DirectXMath.h>
#include "ConstantBuffer.h"
#include "Camera.h"

class Graphics;

class PointLight
{
public:
	PointLight(Graphics& graphics, const DirectX::XMVECTOR& position);
	void SetDiffuseColor(Graphics& graphics, const DirectX::XMFLOAT3& newColor);
	void Bind(Graphics& graphics, const DirectX::XMMATRIX& cameraView);
	DirectX::XMMATRIX GetLightPerspective() const noexcept;
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

	struct ShadowMapBuffer
	{
		DirectX::XMMATRIX lightPerspective;
	} shadowMapBuffer;

	DirectX::XMVECTOR position;
	Camera shadowMapCamera;
	std::unique_ptr<ConstantBuffer<LightBuffer>> constantLightBuffer;
	std::unique_ptr<ConstantBuffer<ShadowMapBuffer>> constantShadowMapBuffer;
};
