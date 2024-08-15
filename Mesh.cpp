#include "Mesh.h"
#include "Graphics.h"
#include <stdexcept>
#include <string>
#include <cassert>
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "InputLayout.h"
#include "PixelShader.h"
#include "VertexShader.h"
#include "ConstantBuffer.h"
#include "ModelsPool.h"
#include "BindablesPool.h"

Mesh::Mesh(Graphics& graphics, const std::string& fileName, const ShaderType shaderType, const DirectX::XMVECTOR& position, const DirectX::XMVECTOR& rotation, const DirectX::XMVECTOR& scale) :
	position(position),
	rotation(rotation),
	scale(scale)
{
	model = ModelsPool::GetInstance().GetModel(fileName);

	std::wstring pixelShaderPath;
	std::wstring vertexShaderPath;

	switch (shaderType)
	{
	case ShaderType::Solid:
		pixelShaderPath = L"PixelShader.cso";
		vertexShaderPath = L"VertexShader.cso";
		break;
	case ShaderType::Phong:
		pixelShaderPath = L"PhongPS.cso";
		vertexShaderPath = L"PhongVS.cso";
		break;
	}
	auto& bindablesPool = BindablesPool::GetInstance();

	bindables.push_back(std::make_unique<ConstantBuffer<ColorBuffer>>(graphics, colorBuffer, BufferType::Pixel));
	sharedBindables.push_back(bindablesPool.GetBindable<PixelShader>(graphics, pixelShaderPath));
	bindables.push_back(std::make_unique<VertexBuffer<Model::Vertex>>(graphics, model->vertices));
	bindables.push_back(std::make_unique<IndexBuffer>(graphics, model->indices));
	bindables.push_back(std::make_unique<ConstantBuffer<TransformBuffer>>(graphics, transformBuffer, BufferType::Vertex));
	auto vertexShader = bindablesPool.GetBindable<VertexShader>(graphics, vertexShaderPath);
	const VertexShader& vertexShaderRef = dynamic_cast<VertexShader&>(*vertexShader);

	std::vector<D3D11_INPUT_ELEMENT_DESC> inputElementDescs =
	{
		{"POSITION", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 0u, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0u},
		{"NORMAL", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 0u, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0u},
	};
	bindables.push_back(std::make_unique<InputLayout>(graphics, inputElementDescs, vertexShaderRef.GetBufferPointer(), vertexShaderRef.GetBufferSize()));
	sharedBindables.push_back(std::move(vertexShader));
}

void Mesh::Draw(Graphics& graphics)
{
	SetTransformBuffer(graphics); // in this case does not have to be updated every frame but wif wee add any movement of camera or object it should be

	for (auto& bindable : bindables)
	{
		bindable->Update(graphics);
		bindable->Bind(graphics);
	}

	for (auto& sharedBindable : sharedBindables)
	{
		sharedBindable->Update(graphics);
		sharedBindable->Bind(graphics);
	}

	graphics.DrawIndexed(model->indices.size());
}

void Mesh::AddRotation(const DirectX::XMVECTOR& rotationToAdd) noexcept
{
	rotation = DirectX::XMVectorAdd(rotation, rotationToAdd);
}

void Mesh::AddPosition(const DirectX::XMVECTOR& posiationToAdd) noexcept
{
	position = DirectX::XMVectorAdd(position, posiationToAdd);
}

void Mesh::Scale(const float scaleFactor) noexcept
{
	scale = DirectX::XMVectorScale(scale, scaleFactor);
}

void Mesh::SetColor(Graphics& graphics, const DirectX::XMFLOAT4& newColor)
{
	colorBuffer = newColor;
}

DirectX::XMFLOAT3 Mesh::GetColor() const noexcept
{
	return DirectX::XMFLOAT3(colorBuffer.color.x, colorBuffer.color.y, colorBuffer.color.z);
}

DirectX::XMFLOAT3 Mesh::GetPosition() const noexcept
{
	DirectX::XMFLOAT3 retPosition;
	DirectX::XMStoreFloat3(&retPosition, position);
	return retPosition;
}

DirectX::XMMATRIX Mesh::GetTransformMatrix() const noexcept
{
	return DirectX::XMMatrixScalingFromVector(scale) * DirectX::XMMatrixRotationRollPitchYawFromVector(rotation) * DirectX::XMMatrixTranslationFromVector(position);
}

void Mesh::SetTransformBuffer(Graphics& graphics)
{
	DirectX::XMMATRIX transformView = DirectX::XMMatrixTranspose(GetTransformMatrix() * graphics.GetCamera());
	DirectX::XMMATRIX transformViewProjection = DirectX::XMMatrixTranspose(GetTransformMatrix() * graphics.GetCamera() * graphics.GetProjection());
	transformBuffer = TransformBuffer(std::move(transformView), std::move(transformViewProjection));
}