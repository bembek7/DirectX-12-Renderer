#pragma once
#include <string>
#include <vector>
#include <DirectXMath.h>
#include <memory>
#include "Bindable.h"
#include "Model.h"
#include "SceneComponent.h"

class Graphics;

enum class ShaderType
{
	Solid,
	Phong
};

class MeshComponent : public SceneComponent
{
public:
	static std::unique_ptr<MeshComponent> CreateComponent(Graphics& graphics, const std::string& fileName, const ShaderType shaderType, const std::string& componentName = "Mesh");
	static std::unique_ptr<MeshComponent> CreateComponent(Graphics& graphics, const aiNode* const node, const aiScene* const scene);

	void Draw(Graphics& graphics);
	void RenderShadowMap(Graphics& graphics);

	void SetColor(Graphics& graphics, const DirectX::XMFLOAT4& newColor);
	DirectX::XMFLOAT3 GetColor() const noexcept;

protected:
	MeshComponent(Graphics& graphics, const std::string& fileName, const ShaderType shaderType, const std::string& componentName);
	MeshComponent(Graphics& graphics, const aiNode* const node, const aiScene* const scene);

private:
	void UpdateTransformBuffer(Graphics& graphics);

private:
	std::vector<std::unique_ptr<Bindable>> bindables;
	std::vector<std::shared_ptr<Bindable>> sharedBindables;
	std::vector<std::unique_ptr<Bindable>> techinquesMutualBindables;
	std::vector<std::shared_ptr<Bindable>> shadowMapSharedBindables;
	std::shared_ptr<Model> model;

	struct TransformBuffer
	{
		TransformBuffer() = default;
		TransformBuffer(const DirectX::XMMATRIX newTransform, const DirectX::XMMATRIX newTransformView, const DirectX::XMMATRIX newTransformViewProjection);
		DirectX::XMFLOAT4X4 transform;
		DirectX::XMFLOAT4X4 transformView;
		DirectX::XMFLOAT4X4 transformViewProjection;
	};
	TransformBuffer transformBuffer = {};

	struct ColorBuffer
	{
		ColorBuffer() = default;
		ColorBuffer(const DirectX::XMFLOAT4 newColor) :
			color(newColor)
		{}
		DirectX::XMFLOAT4 color;
	};
	ColorBuffer colorBuffer = { { 1.0f, 1.0f, 1.0f, 1.0f } };
	bool rendersShadowMap = false;
};
