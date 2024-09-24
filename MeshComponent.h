#pragma once
#include <string>
#include <vector>
#include <DirectXMath.h>
#include <memory>
#include "Model.h"
#include "SceneComponent.h"
#include "Material.h"
#include "ConstantBuffer.h"
#include "ShaderSettings.h"

class Graphics;
class Bindable;

struct aiMaterial;

class MeshComponent : public SceneComponent
{
public:
	static std::unique_ptr<MeshComponent> CreateComponent(Graphics& graphics, const aiNode* const node, const aiScene* const scene);

	void Draw(Graphics& graphics);
	void RenderShadowMap(Graphics& graphics);

	Material* GetMaterial() noexcept;

protected:
	MeshComponent(Graphics& graphics, const aiNode* const node, const aiScene* const scene);

	virtual void RenderComponentDetails(Gui& gui) override;

private:
	void UpdateTransformBuffer(Graphics& graphics);

	static ShaderSettings ResolveShaderSettings(const aiMesh* const mesh, const aiMaterial* const material);

private:
	std::unique_ptr<Model> model;
	std::unique_ptr<Material> material;
	std::shared_ptr<Bindable> nullPixelShader;
	std::unique_ptr<Model> modelForShadowMapping;

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

	bool generatesShadow = false;
};
