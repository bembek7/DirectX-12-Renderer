#include "PointLight.h"
#include "RootParametersDescription.h"

PointLight::PointLight(Graphics& graphics, const std::string& fileName, const std::string& actorName) :
	Light(graphics, fileName, actorName, LightType::Point)
{
	constantBuffers.push_back(std::make_unique<ConstantBufferCBV<LightBuffer>>(graphics, lightBuffer, 0u));
}

void PointLight::Update(Graphics& graphics)
{
	DirectX::XMStoreFloat3(&lightBuffer.lightViewLocation, DirectX::XMVector3Transform(GetActorLocationVector(), graphics.GetCamera()));

	Light::Update(graphics);
}

void PointLight::RenderActorDetails(Gui& gui)
{
	Actor::RenderActorDetails(gui);
	gui.RenderActorDetails(this);
}