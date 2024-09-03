#pragma once
#include <string>
#include <vector>
#include <DirectXMath.h>
#include <memory>
#include "Bindable.h"
#include "Model.h"
#include "SceneComponent.h"
#include "Material.h"
#include "ConstantBuffer.h"

class Graphics;

enum class ShaderType
{
	Solid,
	Phong
};

class MeshComponent : public SceneComponent
{
public:
	static std::unique_ptr<MeshComponent> CreateComponent(Graphics& graphics, const aiNode* const node, const aiScene* const scene);

	void Draw(Graphics& graphics);
	void RenderShadowMap(Graphics& graphics);

protected:
	MeshComponent(Graphics& graphics, const aiNode* const node, const aiScene* const scene);

private:
	void UpdateTransformBuffer(Graphics& graphics);

private:
	//std::vector<std::unique_ptr<Bindable>> bindables;
	//std::vector<std::shared_ptr<Bindable>> sharedBindables;
	//std::vector<std::unique_ptr<Bindable>> techinquesMutualBindables;
	//std::vector<std::shared_ptr<Bindable>> shadowMapSharedBindables;
	std::unique_ptr<Model> model;
	std::unique_ptr<Material> material;

	struct TransformBuffer
	{
		TransformBuffer() = default;
		TransformBuffer(const DirectX::XMMATRIX newTransform, const DirectX::XMMATRIX newTransformView, const DirectX::XMMATRIX newTransformViewProjection);
		DirectX::XMFLOAT4X4 transform;
		DirectX::XMFLOAT4X4 transformView;
		DirectX::XMFLOAT4X4 transformViewProjection;
	};
	TransformBuffer transformBuffer = {};

	std::unique_ptr<ConstantBuffer<TransformBuffer>> transformConstantBuffer;

	bool rendersShadowMap = false;
};
