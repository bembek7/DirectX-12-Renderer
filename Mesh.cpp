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
#include "Utils.h"

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
		rendersShadowMap = true;
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
	sharedBindables.push_back(bindablesPool.GetBindable<InputLayout>(graphics, inputElementDescs, vertexShaderRef.GetBufferPointer(), vertexShaderRef.GetBufferSize(), WstringToString(vertexShaderPath)));
	sharedBindables.push_back(std::move(vertexShader));

	if (shaderType == ShaderType::Phong)
	{
		auto vertexShaderSM = bindablesPool.GetBindable<VertexShader>(graphics, L"VertexShader.cso");
		const VertexShader& vertexShaderRefSM = dynamic_cast<VertexShader&>(*vertexShaderSM);
		std::vector<D3D11_INPUT_ELEMENT_DESC> inputElementDescsSM =
		{
			{"POSITION", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 0u, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0u}
		};
		shadowMapSharedBindables.push_back(bindablesPool.GetBindable<InputLayout>(graphics, inputElementDescsSM, vertexShaderRefSM.GetBufferPointer(), vertexShaderRefSM.GetBufferSize(), WstringToString(L"VertexShader.cso")));
		shadowMapSharedBindables.push_back(std::move(vertexShaderSM));
		shadowMapSharedBindables.push_back(bindablesPool.GetBindable<PixelShader>(graphics, L""));
	}
}

void Mesh::Draw(Graphics& graphics)
{
	SetTransformBuffer(graphics);

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

void Mesh::RenderShadowMap(Graphics& graphics)
{
	if (rendersShadowMap)
	{
		SetTransformBuffer(graphics);

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

		for (auto& bindable : shadowMapSharedBindables)
		{
			bindable->Update(graphics);
			bindable->Bind(graphics);
		}

		graphics.DrawIndexed(model->indices.size());
	}
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
	transformBuffer = TransformBuffer(DirectX::XMMatrixTranspose(GetTransformMatrix()), std::move(transformView), std::move(transformViewProjection));
}