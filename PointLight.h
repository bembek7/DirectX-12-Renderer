#pragma once
#include <DirectXMath.h>
#include "ConstantBuffer.h"
#include "Camera.h"
#include "MeshActor.h"

class Graphics;

class PointLight : public MeshActor
{
public:
	PointLight(Graphics& graphics, const std::string& fileName, const std::string& actorName = "Point Light");
	void SetDiffuseColor(Graphics& graphics, const DirectX::XMFLOAT3 newColor);
	void Bind(Graphics& graphics, const DirectX::XMMATRIX cameraView);
	DirectX::XMMATRIX GetLightPerspective() const noexcept;
private:
	struct LightBuffer
	{
		DirectX::XMFLOAT3 diffuseColor = { 1.0f, 1.0f, 1.0f };
		float diffuseIntensity = 0.6f;
		DirectX::XMFLOAT3 ambient = { 0.15f, 0.15f, 0.15f };
		float specularIntensity = 0.6f;
		DirectX::XMFLOAT3 lightViewLocation = { 0.f, 0.f, 0.f };
		float specularPower = 0.8f;
	} lightBuffer;

	struct ShadowMapBuffer
	{
		DirectX::XMFLOAT4X4 lightPerspective;
	} shadowMapBuffer;

	Camera* shadowMapCamera;
	std::unique_ptr<ConstantBuffer<LightBuffer>> constantLightBuffer;
	std::unique_ptr<ConstantBuffer<ShadowMapBuffer>> constantShadowMapBuffer;
};
