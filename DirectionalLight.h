#pragma once
#include "Light.h"

class Graphics;

class DirectionalLight : public Light
{
	friend class Gui;
public:
	DirectionalLight(Graphics& graphics, const std::string& fileName, const std::string& actorName = "Directional Light");
	virtual void Update(Graphics& graphics) override;

protected:
	virtual void RenderActorDetails(Gui& gui) override;
private:
	DirectX::XMFLOAT4X4 projection{};
	struct LightBuffer
	{
		DirectX::XMFLOAT3 diffuseColor = { 1.0f, 1.0f, 1.0f };
		float diffuseIntensity = 0.5f;
		DirectX::XMFLOAT3 ambient = { 0.1f, 0.1f, 0.1f };
		float specularIntensity = 0.6f;
		DirectX::XMFLOAT3 lightDirection = { 0.f, 0.f, 0.f };
		char padding[4];
	} lightBuffer;
};
