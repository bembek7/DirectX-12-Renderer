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
#include "PipelineState.h"
#include "RootSignature.h"

class Graphics;
class Bindable;

struct aiMaterial;

class MeshComponent : public SceneComponent
{
public:
	static std::unique_ptr<MeshComponent> CreateComponent(Graphics& graphics, const aiNode* const node, const aiScene* const scene);
	virtual void Draw(Graphics& graphics, const std::vector<Light*>& lights) override;
	virtual void Update(Graphics& graphics) override;
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
	std::unique_ptr<Model> modelForShadowMapping;
	std::shared_ptr<PipelineState> pipelineState;
	std::shared_ptr<PipelineState> smPipelineState;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> drawingBundle;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> shadowMappingBundle;

	struct TransformBuffer
	{
		TransformBuffer() = default;
		TransformBuffer(const DirectX::XMMATRIX newTransform, const DirectX::XMMATRIX newView, const DirectX::XMMATRIX newProjection);
		DirectX::XMFLOAT4X4 transform;
		DirectX::XMFLOAT4X4 view;
		DirectX::XMFLOAT4X4 projection;
	};
	TransformBuffer transformBuffer = {};

	std::unique_ptr<ConstantBuffer<TransformBuffer>> transformConstantBuffer;

	bool lighted = false;
};
