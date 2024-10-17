#include "DirectionalLight.h"
#include "Graphics.h"
#include "RootParametersDescription.h"

DirectionalLight::DirectionalLight(Graphics& graphics, const std::string& fileName, const std::string& actorName) :
	Light(graphics, fileName, actorName)
{
	constantBuffers.push_back(std::make_unique<ConstantBuffer<LightBuffer>>(graphics, lightBuffer, RPD::Light));
}

void DirectionalLight::Update(Graphics& graphics)
{
	DirectX::XMStoreFloat3(&lightBuffer.lightDirection, GetActorRotationRadians());

	Light::Update(graphics);
}

void DirectionalLight::RenderActorDetails(Gui& gui)
{
	Actor::RenderActorDetails(gui);
	gui.RenderActorDetails(this);
}