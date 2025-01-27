#include "Light.h"
#include "Camera.h"
#include "RootParametersDescription.h"

Light::Light(Graphics& graphics, const std::string& fileName, const std::string& actorName, const LightType type) :
	MeshActor(graphics, fileName, actorName),
	type(type)
{
	shadowMapCamera = SceneComponent::AttachComponents<Camera>(std::move(Camera::CreateComponent("Shadow Map Camera")), rootComponent.get());

	constantBuffers.push_back(std::make_unique<ConstantBufferCBV<LightPerspectiveBuffer>>(graphics, lightPerspectiveBuffer, 1u)); // TODO removeMagicNumber
}

void Light::Bind(ID3D12GraphicsCommandList* const commandList)
{
	for (auto& cB : constantBuffers)
	{
		cB->Bind(commandList);
	}
}

void Light::Update(Graphics& graphics)
{
	Actor::Update(graphics);

	DirectX::XMStoreFloat4x4(&lightPerspectiveBuffer.view, shadowMapCamera->GetMatrix());
	lightPerspectiveBuffer.projection = projection;

	for (auto& cB : constantBuffers)
	{
		cB->Update();
	}
}

const Camera* Light::GetLightCamera() const noexcept
{
	return shadowMapCamera;
}

DirectX::XMFLOAT4X4 Light::GetLightProjection() const noexcept
{
	return projection;
}

LightType Light::GetType() const noexcept
{
	return type;
}
