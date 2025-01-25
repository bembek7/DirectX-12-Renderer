#include "SpotLight.h"
#include "RootParametersDescription.h"

SpotLight::SpotLight(Graphics& graphics, const std::string& fileName, const std::string& actorName) :
	Light(graphics, fileName, actorName, LightType::Spot)
{
	constantBuffers.push_back(std::make_unique<ConstantBufferCBV<LightBuffer>>(graphics, lightBuffer, 0u));
}

void SpotLight::Update(Graphics& graphics)
{
	namespace Dx = DirectX;

	Dx::XMStoreFloat3(&lightBuffer.lightViewLocation, Dx::XMVector3Transform(GetActorLocationVector(), graphics.GetCamera()));
	Dx::XMStoreFloat3(&lightBuffer.lightDirection, Dx::XMVector3TransformNormal(GetActorForwardVector(), graphics.GetCamera()));

	Light::Update(graphics);
}

void SpotLight::RenderActorDetails(Gui& gui)
{
	Actor::RenderActorDetails(gui);
	gui.RenderActorDetails(this);
}