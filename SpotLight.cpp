#include "SpotLight.h"
#include "RootParametersDescription.h"

SpotLight::SpotLight(Graphics& graphics, const std::string& actorName) :
	Light(graphics, actorName, LightType::Spot)
{
	constantBuffers.push_back(std::make_unique<ConstantBufferCBV<LightBuffer>>(graphics, lightBuffer, 0u));

	namespace Dx = DirectX;
	Dx::XMMATRIX reverseZ =
	{
		1.0f, 0.0f,  0.0f, 0.0f,
		0.0f, 1.0f,  0.0f, 0.0f,
		0.0f, 0.0f, -1.0f, 0.0f,
		0.0f, 0.0f,  1.0f, 1.0f
	};
	const float windowWidth = graphics.GetWindowWidth();
	const float windowHeight = graphics.GetWindowHeight();

	Dx::XMStoreFloat4x4(&projection, Dx::XMMatrixPerspectiveLH(1.0f, 1.0f, 0.1f, 100.0f) * reverseZ);
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