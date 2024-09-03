#include "BetterWindows.h"
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

#include <assimp/scene.h>
#include <assimp/postprocess.h>

MeshComponent::MeshComponent(Graphics& graphics, const aiNode* const node, const aiScene* const scene) :
	SceneComponent(graphics, node, scene)
{
	if (node->mNumMeshes > 1)
	{
		throw std::runtime_error("Nodes with more than one mesh not handled yet");
	}

	const unsigned int meshIndex = *node->mMeshes;

	const aiMesh* const assignedMesh = scene->mMeshes[meshIndex];
	model = std::make_unique<Model>(graphics, assignedMesh);

	const aiMaterial* const assignedMaterial = scene->mMaterials[assignedMesh->mMaterialIndex];
	material = std::make_unique<Material>(graphics, assignedMaterial);

	rendersShadowMap = true;

	transformConstantBuffer = std::make_unique<ConstantBuffer<TransformBuffer>>(graphics, transformBuffer, BufferType::Vertex, 0u);
	//techinquesMutualBindables.push_back(std::make_unique<ConstantBuffer<TransformBuffer>>(graphics, transformBuffer, BufferType::Vertex));

	/*if (rendersShadowMap)
	{
		auto& bindablesPool = BindablesPool::GetInstance();
		shadowMapSharedBindables.push_back(bindablesPool.GetBindable<PixelShader>(graphics, L""));
		auto vertexShaderSM = bindablesPool.GetBindable<VertexShader>(graphics, L"VertexShader.cso");
		const VertexShader& vertexShaderRefSM = dynamic_cast<VertexShader&>(*vertexShaderSM);
		std::vector<D3D11_INPUT_ELEMENT_DESC> inputElementDescsSM =
		{
			{"POSITION", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 0u, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0u}
		};
		shadowMapSharedBindables.push_back(bindablesPool.GetBindable<InputLayout>(graphics, inputElementDescsSM, vertexShaderRefSM.GetBufferPointer(), vertexShaderRefSM.GetBufferSize(), WstringToString(L"VertexShader.cso")));
		shadowMapSharedBindables.push_back(std::move(vertexShaderSM));
	}*/
}

std::unique_ptr<MeshComponent> MeshComponent::CreateComponent(Graphics& graphics, const aiNode* const node, const aiScene* const scene)
{
	return std::unique_ptr<MeshComponent>(new MeshComponent(graphics, node, scene));
}

void MeshComponent::Draw(Graphics& graphics)
{
	UpdateTransformBuffer(graphics);

	model->Bind(graphics);
	material->Bind(graphics);
	transformConstantBuffer->Update(graphics);
	transformConstantBuffer->Bind(graphics);

	graphics.DrawIndexed(model->GetIndicesNumber());
}

void MeshComponent::RenderShadowMap(Graphics& graphics)
{
	if (rendersShadowMap)
	{
		UpdateTransformBuffer(graphics);

		model->Bind(graphics);
		material->Bind(graphics);
		transformConstantBuffer->Update(graphics);
		transformConstantBuffer->Bind(graphics);

		graphics.DrawIndexed(model->GetIndicesNumber());
	}
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