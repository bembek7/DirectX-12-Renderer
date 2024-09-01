#include "MeshComponent.h"
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
#include "SceneComponent.h"

MeshComponent::MeshComponent(Graphics& graphics, const std::string& fileName, const ShaderType shaderType, const std::string& componentName) :
	SceneComponent(componentName)
{
	model = ModelsPool::GetInstance().GetModel(fileName);

	std::wstring pixelShaderPath;
	std::wstring vertexShaderPath;

	std::vector<D3D11_INPUT_ELEMENT_DESC> inputElementDescs;

	switch (shaderType)
	{
	case ShaderType::Solid:
		pixelShaderPath = L"PixelShader.cso";
		vertexShaderPath = L"VertexShader.cso";
		inputElementDescs =
		{
			{"POSITION", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 0u, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0u},
		};
		break;
	case ShaderType::Phong:
		rendersShadowMap = true;
		pixelShaderPath = L"PhongPS.cso";
		vertexShaderPath = L"PhongVS.cso";
		inputElementDescs =
		{
			{"POSITION", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 0u, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0u},
			{"NORMAL", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 0u, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0u},
		};
		break;
	}

	techinquesMutualBindables.push_back(std::make_unique<VertexBuffer<Model::Vertex>>(graphics, model->vertices));
	techinquesMutualBindables.push_back(std::make_unique<IndexBuffer>(graphics, model->indices));
	techinquesMutualBindables.push_back(std::make_unique<ConstantBuffer<TransformBuffer>>(graphics, transformBuffer, BufferType::Vertex));

	bindables.push_back(std::make_unique<ConstantBuffer<ColorBuffer>>(graphics, colorBuffer, BufferType::Pixel));

	auto& bindablesPool = BindablesPool::GetInstance();

	sharedBindables.push_back(bindablesPool.GetBindable<PixelShader>(graphics, pixelShaderPath));
	auto vertexShader = bindablesPool.GetBindable<VertexShader>(graphics, vertexShaderPath);
	const VertexShader& vertexShaderRef = dynamic_cast<VertexShader&>(*vertexShader);
	sharedBindables.push_back(bindablesPool.GetBindable<InputLayout>(graphics, inputElementDescs, vertexShaderRef.GetBufferPointer(), vertexShaderRef.GetBufferSize(), WstringToString(vertexShaderPath)));
	sharedBindables.push_back(std::move(vertexShader));

	if (rendersShadowMap)
	{
		shadowMapSharedBindables.push_back(bindablesPool.GetBindable<PixelShader>(graphics, L""));
		auto vertexShaderSM = bindablesPool.GetBindable<VertexShader>(graphics, L"VertexShader.cso");
		const VertexShader& vertexShaderRefSM = dynamic_cast<VertexShader&>(*vertexShaderSM);
		std::vector<D3D11_INPUT_ELEMENT_DESC> inputElementDescsSM =
		{
			{"POSITION", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 0u, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0u}
		};
		shadowMapSharedBindables.push_back(bindablesPool.GetBindable<InputLayout>(graphics, inputElementDescsSM, vertexShaderRefSM.GetBufferPointer(), vertexShaderRefSM.GetBufferSize(), WstringToString(L"VertexShader.cso")));
		shadowMapSharedBindables.push_back(std::move(vertexShaderSM));
	}
}

std::unique_ptr<MeshComponent> MeshComponent::CreateComponent(Graphics& graphics, const std::string& fileName, const ShaderType shaderType, const std::string& componentName)
{
	return std::unique_ptr<MeshComponent>(new MeshComponent(graphics, fileName, shaderType, componentName));
}

void MeshComponent::Draw(Graphics& graphics)
{
	UpdateTransformBuffer(graphics);

	for (auto& bindable : bindables)
	{
		bindable->Update(graphics);
		bindable->Bind(graphics);
	}

	for (auto& bindable : techinquesMutualBindables)
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

void MeshComponent::RenderShadowMap(Graphics& graphics)
{
	if (rendersShadowMap)
	{
		UpdateTransformBuffer(graphics);

		for (auto& bindable : techinquesMutualBindables)
		{
			bindable->Update(graphics);
			bindable->Bind(graphics);
		}

		for (auto& sharedBindable : shadowMapSharedBindables)
		{
			sharedBindable->Update(graphics);
			sharedBindable->Bind(graphics);
		}

		graphics.DrawIndexed(model->indices.size());
	}
}

void MeshComponent::SetColor(Graphics& graphics, const DirectX::XMFLOAT4& newColor)
{
	colorBuffer = newColor;
}

DirectX::XMFLOAT3 MeshComponent::GetColor() const noexcept
{
	return DirectX::XMFLOAT3(colorBuffer.color.x, colorBuffer.color.y, colorBuffer.color.z);
}

void MeshComponent::UpdateTransformBuffer(Graphics& graphics)
{
	DirectX::XMMATRIX transformMatrix = GetTransformMatrix();
	DirectX::XMMATRIX transformView = DirectX::XMMatrixTranspose(transformMatrix * graphics.GetCamera());
	DirectX::XMMATRIX transformViewProjection = DirectX::XMMatrixTranspose(transformMatrix * graphics.GetCamera() * graphics.GetProjection());
	transformBuffer = TransformBuffer(DirectX::XMMatrixTranspose(transformMatrix), std::move(transformView), std::move(transformViewProjection));
}

MeshComponent::TransformBuffer::TransformBuffer(const DirectX::XMMATRIX newTransform, const DirectX::XMMATRIX newTransformView, const DirectX::XMMATRIX newTransformViewProjection)
{
	DirectX::XMStoreFloat4x4(&transform, newTransform);
	DirectX::XMStoreFloat4x4(&transformView, newTransformView);
	DirectX::XMStoreFloat4x4(&transformViewProjection, newTransformViewProjection);
}