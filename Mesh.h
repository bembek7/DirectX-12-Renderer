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

class Mesh
{
public:
	Mesh(Graphics& graphics, const std::string& fileName, const ShaderType shaderType);

	void Draw(Graphics& graphics);
	void RenderShadowMap(Graphics& graphics);

	void SetColor(Graphics& graphics, const DirectX::XMFLOAT4& newColor);
	DirectX::XMFLOAT3 GetColor() const noexcept;

	void SetTransform(DirectX::XMMATRIX newTransform) noexcept;

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
	TransformBuffer transformBuffer;

	struct ColorBuffer
	{
		ColorBuffer() = default;
		ColorBuffer(const DirectX::XMFLOAT4 newColor) :
			color(newColor)
		{}
		DirectX::XMFLOAT4 color;
	};
	ColorBuffer colorBuffer = { { 1.0f, 1.0f, 1.0f, 1.0f } };
	DirectX::XMFLOAT4X4 transform;
	bool rendersShadowMap = false;
};
