#pragma once
#include <DirectXMath.h>
#include "Light.h"

class PointLight : public Light
{
	friend class Gui;
public:
	PointLight(Graphics& graphics, const std::string& actorName = "Point Light");
	virtual void Update(Graphics& graphics) override;

protected:
	virtual void RenderActorDetails(Gui& gui) override;
private:

	struct LightBuffer
	{
		DirectX::XMFLOAT3 diffuseColor = { 1.0f, 1.0f, 1.0f };
		float diffuseIntensity = 1.0f;
		DirectX::XMFLOAT3 ambient = { 0.05f, 0.05f, 0.05f };
		float specularIntensity = 0.6f;
		DirectX::XMFLOAT3 lightViewLocation = { 0.f, 0.f, 0.f };
		float attenuationConst = 1.0f;
		float attenuationLin = 0.095f;
		float attenuationQuad = 0.0040f;
		char padding[8];
	} lightBuffer;
};
