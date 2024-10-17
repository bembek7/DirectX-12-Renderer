#include "PointLight.h"
#include "RootParametersDescription.h"

PointLight::PointLight(Graphics& graphics, const std::string& fileName, const std::string& actorName) :
	Light(graphics, fileName, actorName)
{
	constantBuffers.push_back(std::make_unique<ConstantBuffer<LightBuffer>>(graphics, lightBuffer, RPD::Light));
}

void PointLight::Update(Graphics& graphics)
{
	//DirectX::XMStoreFloat3(&lightBuffer.lightViewLocation, DirectX::XMVector3Transform(GetActorLocationVector(), graphics.GetCamera()));
	DirectX::XMStoreFloat3(&lightBuffer.lightDirection, GetActorRotationRadians());

	Light::Update(graphics);
}

void PointLight::RenderActorDetails(Gui& gui)
{
	Actor::RenderActorDetails(gui);
	gui.RenderActorDetails(this);
}