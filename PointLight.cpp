#include "PointLight.h"
#include "RootParametersDescription.h"

PointLight::PointLight(Graphics& graphics, const std::string& actorName) :
	Light(graphics, actorName, LightType::Point)
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

	Dx::XMStoreFloat4x4(&projection, Dx::XMMatrixPerspectiveLH(1.0f, 1.0f, 0.5f, 100.0f) * reverseZ);
}

void PointLight::Update(Graphics& graphics)
{
	DirectX::XMStoreFloat3(&lightBuffer.lightViewLocation, DirectX::XMVector3Transform(GetActorLocationVector(), graphics.GetCamera()));
	lightBuffer.lightWorldLocation = GetActorLocation();
	Light::Update(graphics);
}

void PointLight::RenderActorDetails(Gui& gui)
{
	Actor::RenderActorDetails(gui);
	gui.RenderActorDetails(this);
}