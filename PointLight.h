#pragma once
#include <DirectXMath.h>
#include "ConstantBuffer.h"
#include "MeshActor.h"

class Graphics;
class Camera;

class PointLight : public MeshActor
{
	friend class Gui;
public:
	PointLight(Graphics& graphics, const std::string& fileName, const std::string& actorName = "Point Light");
	void SetDiffuseColor(Graphics& graphics, const DirectX::XMFLOAT3 newColor);
	void Bind(Graphics& graphics, ID3D12GraphicsCommandList* const commandList);
	void Update(Graphics& graphics);
	DirectX::XMMATRIX GetLightPerspective() const noexcept;

protected:
	virtual void RenderActorDetails(Gui& gui) override;
private:
	struct LightBuffer
	{
		DirectX::XMFLOAT3 diffuseColor = { 1.0f, 1.0f, 1.0f };
		float diffuseIntensity = 1.0f;
		DirectX::XMFLOAT3 ambient = { 0.25f, 0.25f, 0.25f };
		float specularIntensity = 0.6f;
		DirectX::XMFLOAT3 lightViewLocation = { 0.f, 0.f, 0.f };
		const float attenuationConst = 1.0f;
		const float attenuationLin = 0.015f;
		const float attenuationQuad = 0.0020f;
		char padding[8];
	} lightBuffer;

	struct ShadowMapBuffer
	{
		DirectX::XMFLOAT4X4 lightPerspective;
	} shadowMapBuffer;

	Camera* shadowMapCamera;
	std::unique_ptr<ConstantBuffer<LightBuffer>> constantLightBuffer;
	std::unique_ptr<ConstantBuffer<ShadowMapBuffer>> constantShadowMapBuffer;
};
