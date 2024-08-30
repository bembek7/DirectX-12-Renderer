#include "MeshComponent.h"

std::unique_ptr<MeshComponent> MeshComponent::CreateComponent(const std::string& componentName)
{
	return std::unique_ptr<MeshComponent>(new MeshComponent(componentName));
}

//Mesh* MeshComponent::GetMesh() const noexcept
//{
//	return mesh.get();
//}

void MeshComponent::SetMesh(std::unique_ptr<Mesh> newMesh) noexcept
{
	mesh = std::move(newMesh);
}

void MeshComponent::Draw(Graphics& graphics)
{
	SceneComponent::Draw(graphics);

	mesh->SetTransform(GetTransformMatrix());
	mesh->Draw(graphics);
}

void MeshComponent::RenderShadowMap(Graphics& graphics)
{
	SceneComponent::RenderShadowMap(graphics);

	mesh->SetTransform(GetTransformMatrix());
	mesh->RenderShadowMap(graphics);
}