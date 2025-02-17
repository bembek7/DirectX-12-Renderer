#include "DirectionalLight.h"
#include "Graphics.h"
#include "RootParametersDescription.h"
#include "Camera.h"

DirectionalLight::DirectionalLight(Graphics& graphics, const std::string& actorName) :
	Light(graphics, actorName, LightType::Directional)
{
	constantBuffers.push_back(std::make_unique<ConstantBufferCBV<LightBuffer>>(graphics, lightBuffer, 0u)); // TODO get rid of mgic number

	const float windowWidth = graphics.GetWindowWidth();
	const float windowHeight = graphics.GetWindowHeight();

	namespace Dx = DirectX;
	Dx::XMStoreFloat4x4(&projection, Dx::XMMatrixOrthographicLH(350.0f, 350.0f, 700.0f, 0.5f));
}

void DirectionalLight::Update(Graphics& graphics)
{
	namespace Dx = DirectX;
	const auto lightDir = GetActorForwardVector();
	const auto lightLocation = Dx::XMVectorScale(lightDir, -300);
	
	shadowMapCamera->SetRelativeLocation(Dx::XMVectorScale(GetActorForwardVector(), -200));
	Dx::XMStoreFloat3(&lightBuffer.lightDirection, Dx::XMVector3TransformNormal(GetActorForwardVector(), graphics.GetCamera()));
	Light::Update(graphics);
}

void DirectionalLight::RenderActorDetails(Gui& gui)
{
	Actor::RenderActorDetails(gui);
	gui.RenderActorDetails(this);
}