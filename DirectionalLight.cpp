#include "DirectionalLight.h"
#include "Graphics.h"
#include "RootParametersDescription.h"
#include "Camera.h"

DirectionalLight::DirectionalLight(Graphics& graphics, const std::string& fileName, const std::string& actorName) :
	Light(graphics, fileName, actorName)
{
	constantBuffers.push_back(std::make_unique<ConstantBufferCBV<LightBuffer>>(graphics, lightBuffer, RPD::DirectionalLight));

	const float windowWidth = graphics.GetWindowWidth();
	const float windowHeight = graphics.GetWindowHeight();

	namespace Dx = DirectX;
	Dx::XMStoreFloat4x4(&projection, Dx::XMMatrixOrthographicLH(300.0f, 300 * windowHeight / windowWidth, 0.5f, 700.0f));
}

void DirectionalLight::Update(Graphics& graphics)
{
	namespace Dx = DirectX;
	const auto lightDir = GetActorForwardVector();
	const auto lightLocation = Dx::XMVectorScale(lightDir, -300);
	//shadowMapCamera->SetRelativeLocation(Dx::XMVectorSubtract(lightLocation, GetActorLocationVector()));
	Dx::XMStoreFloat3(&lightBuffer.lightDirection, Dx::XMVector3TransformNormal(GetActorForwardVector(), graphics.GetCamera()));
	//DirectX::XMStoreFloat4x4(&shadowMapBuffer.lightPerspective, DirectX::XMMatrixTranspose(shadowMapCamera->GetMatrix() * Dx::XMLoadFloat4x4(&projection)));
	Light::Update(graphics);
}

void DirectionalLight::RenderActorDetails(Gui& gui)
{
	Actor::RenderActorDetails(gui);
	gui.RenderActorDetails(this);
}