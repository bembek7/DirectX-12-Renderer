#pragma once
#include "Mesh.h"
#include "SceneComponent.h"

class MeshComponent : public SceneComponent
{
public:
	static std::unique_ptr<MeshComponent> CreateComponent(const std::string& componentName = "Mesh");
	//Mesh* GetMesh() const noexcept;
	void SetMesh(std::unique_ptr<Mesh> newMesh) noexcept;

	virtual void Draw(Graphics& graphics) override;
	virtual void RenderShadowMap(Graphics& graphics) override;
protected:
	using SceneComponent::SceneComponent;

protected:
	std::unique_ptr<Mesh> mesh;
};
