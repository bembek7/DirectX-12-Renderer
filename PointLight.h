#pragma once
#include <DirectXMath.h>
#include "Light.h"

class PointLight : public Light
{
	friend class Gui;
public:
	PointLight(Graphics& graphics, const std::string& fileName, const std::string& actorName = "Point Light");
	virtual void Update(Graphics& graphics) override;

protected:
	virtual void RenderActorDetails(Gui& gui) override;
private:

	//	struct LightBuffer
	//	{
	//		DirectX::XMFLOAT3 diffuseColor = { 1.0f, 1.0f, 1.0f };
	//		float diffuseIntensity = 1.0f;
	//		DirectX::XMFLOAT3 ambient = { 0.25f, 0.25f, 0.25f };
	//		float specularIntensity = 0.6f;
	//		DirectX::XMFLOAT3 lightViewLocation = { 0.f, 0.f, 0.f };
	//		const float attenuationConst = 1.0f;
	//		const float attenuationLin = 0.015f;
	//		const float attenuationQuad = 0.0020f;
	//		char padding[8];
	//	} lightBuffer;

	struct LightBuffer
	{
		DirectX::XMFLOAT3 diffuseColor = { 1.0f, 1.0f, 1.0f };
		float diffuseIntensity = 1.0f;
		DirectX::XMFLOAT3 ambient = { 0.25f, 0.25f, 0.25f };
		float specularIntensity = 0.6f;
		DirectX::XMFLOAT3 lightDirection = { 0.f, 0.f, 0.f };
		char padding[4];
	} lightBuffer;
};
