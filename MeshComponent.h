#pragma once
#include <string>
#include <vector>
#include <DirectXMath.h>
#include <memory>
#include <assimp\vector3.h>
#include "Model.h"
#include "SceneComponent.h"
#include "Material.h"
#include "ConstantBuffer.h"
#include "ShaderSettings.h"
#include "PipelineState.h"
#include "RootSignature.h"

class Graphics;
class Bindable;

struct aiMaterial;

class MeshComponent : public SceneComponent
{
public:
	static std::unique_ptr<MeshComponent> CreateComponent(Graphics& graphics, const aiNode* const node, const aiScene* const scene);
	virtual void Draw(Graphics& graphics, const PassType& passType) override;
	void PrepareForPass(Graphics& graphics, Pass* const pass);
	virtual void Update(Graphics& graphics) override;

	Material* GetMaterial() noexcept;

protected:
	MeshComponent(Graphics& graphics, const aiNode* const node, const aiScene* const scene);

	virtual void RenderComponentDetails(Gui& gui) override;

	virtual void ComponentMoved() override;

private:
	void UpdateTransformBuffer(Graphics& graphics);

	void PrepareForGPass(Graphics& graphics, Pass* const pass);
	void PrepareForLightPerspectivePass(Graphics& graphics, Pass* const pass);

	bool IsInsideFrustum(const Graphics::Frustum& frustum) const;

	static ShaderSettings ResolveShaderSettings(const aiMesh* const mesh, const aiMaterial* const material);

	struct BoundingSphere {
		DirectX::XMFLOAT3 center;
		float radius;
	};

	BoundingSphere CalculateModelBoundingSphere(const aiVector3D* const vertices, unsigned int verticesNum) const;
	void UpdateWorldBoundingSphere();
private:
	std::unique_ptr<Model> mainModel;
	std::unique_ptr<Model> primitiveModel;
	std::unique_ptr<Material> mainMaterial;
	std::unique_ptr<PipelineState> mainPipelineState;
	std::unordered_map<PassType, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>> drawingBundles;

	struct TransformBuffer
	{
		TransformBuffer() = default;
		TransformBuffer(const DirectX::XMMATRIX newTransform, const DirectX::XMMATRIX newView, const DirectX::XMMATRIX newProjection);
		DirectX::XMFLOAT4X4 transform;
		DirectX::XMFLOAT4X4 view;
		DirectX::XMFLOAT4X4 projection;
	};
	TransformBuffer transformBuffer = {};

	BoundingSphere modelBoundingSphere;
	BoundingSphere worldBoundingSphere;

	std::unique_ptr<ConstantBufferConstants<TransformBuffer>> transformConstantBuffer;

	bool lighted = false;
};
