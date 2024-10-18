#include "DirectionalLight.h"
#include "Graphics.h"
#include "RootParametersDescription.h"

DirectionalLight::DirectionalLight(Graphics& graphics, const std::string& fileName, const std::string& actorName) :
	Light(graphics, fileName, actorName)
{
	constantBuffers.push_back(std::make_unique<ConstantBuffer<LightBuffer>>(graphics, lightBuffer, RPD::DirectionalLight));
}

void DirectionalLight::Update(Graphics& graphics)
{
	namespace Dx = DirectX;

	Dx::XMStoreFloat3(&lightBuffer.lightDirection, Dx::XMVector3TransformNormal(GetActorForwardVector(), graphics.GetCamera()));

	Light::Update(graphics);
}

void DirectionalLight::RenderActorDetails(Gui& gui)
{
	Actor::RenderActorDetails(gui);
	gui.RenderActorDetails(this);
}