#pragma once
#include "Model.h"
#include "Material.h"
#include <memory>
#include "ConstantBuffer.h"

class Graphics;

class Skybox
{
public:
	Skybox(Graphics& graphics, const std::string& fileName);

	void Draw(Graphics& graphics);
private:
	struct TransformBuffer
	{
		DirectX::XMFLOAT4X4 viewProjection;
	};
	TransformBuffer transformBuffer = {};
	std::unique_ptr<ConstantBuffer<TransformBuffer>> transformConstantBuffer;

	std::unique_ptr<Model> model;
	std::unique_ptr<Material> material;

	std::unique_ptr<DepthStencilState> skyDepthStencilState;
};
