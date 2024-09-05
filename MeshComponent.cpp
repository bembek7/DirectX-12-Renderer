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
		OutputDebugString("Nodes with more than one mesh not handled yet\n");
	}

	const unsigned int meshIndex = *node->mMeshes;

	const aiMesh* const assignedMesh = scene->mMeshes[meshIndex];
	const aiMaterial* const assignedMaterial = scene->mMaterials[assignedMesh->mMaterialIndex];

	aiString texFileName;
	assignedMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &texFileName);

	int shadingModel = 0;
	assignedMaterial->Get(AI_MATKEY_SHADING_MODEL, shadingModel);

	const bool hasTexture = (texFileName.length > 0);
	usesPhong = (shadingModel == aiShadingMode_Phong);

	if (hasTexture && !usesPhong)
	{
		throw std::runtime_error("Meshes not using phong but using texture aren't implement yet, it just needs a bit of boilerplating");
	}

	model = std::make_unique<Model>(graphics, assignedMesh, hasTexture, usesPhong);
	material = std::make_unique<Material>(graphics, assignedMaterial, usesPhong);

	transformConstantBuffer = std::make_unique<ConstantBuffer<TransformBuffer>>(graphics, transformBuffer, BufferType::Vertex, 0u);

	if (usesPhong)
	{
		modelForShadowMapping = std::make_unique<Model>(graphics, assignedMesh, false, false, model->ShareIndexBuffer());
		auto& bindablesPool = BindablesPool::GetInstance();
		nullPixelShader = bindablesPool.GetBindable<PixelShader>(graphics, L"");
	}
}

void MeshComponent::RenderComponentDetails(Gui& gui)
{
	gui.RenderComponentDetails(this);
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
	if (usesPhong)
	{
		UpdateTransformBuffer(graphics);
		modelForShadowMapping->Bind(graphics);
		nullPixelShader->Bind(graphics);
		transformConstantBuffer->Update(graphics);
		transformConstantBuffer->Bind(graphics);

		graphics.DrawIndexed(modelForShadowMapping->GetIndicesNumber());
	}
}

Material* MeshComponent::GetMaterial() noexcept
{
	return material.get();
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