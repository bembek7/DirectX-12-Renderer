#include "BetterWindows.h"
#include "MeshComponent.h"
#include "Graphics.h"
#include <stdexcept>
#include <string>
#include <cassert>
#include "Bindable.h"
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "PipelineStatesPool.h"
#include "RootParametersDescription.h"
#include "Light.h"
#include <d3d12.h>

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

	ShaderSettings shaderSettings = ResolveShaderSettings(assignedMesh, assignedMaterial);

	const auto& transformInfo = RPD::cbsInfo.at(RPD::CBTypes::Transform);
	transformConstantBuffer = std::make_unique<ConstantBufferConstants<TransformBuffer>>(transformBuffer, 0u);

	modelBoundingSphere = CalculateModelBoundingSphere(assignedMesh->mVertices, assignedMesh->mNumVertices);
	UpdateWorldBoundingSphere();
	primitiveModel = std::make_unique<Model>(graphics, assignedMesh, ShaderSettings{});
	mainModel = std::make_unique<Model>(graphics, assignedMesh, shaderSettings, primitiveModel->ShareIndexBuffer());
	mainMaterial = std::make_unique<Material>(graphics, assignedMaterial, shaderSettings);
}

void MeshComponent::RenderComponentDetails(Gui& gui)
{
	SceneComponent::RenderComponentDetails(gui);
	gui.RenderComponentDetails(this);
}

void MeshComponent::ComponentMoved()
{
	SceneComponent::ComponentMoved();
	UpdateWorldBoundingSphere();
}

std::unique_ptr<MeshComponent> MeshComponent::CreateComponent(Graphics& graphics, const aiNode* const node, const aiScene* const scene)
{
	return std::unique_ptr<MeshComponent>(new MeshComponent(graphics, node, scene));
}

void MeshComponent::Draw(Graphics& graphics, const PassType& passType)
{
	SceneComponent::Draw(graphics, passType);

	if (!IsInsideFrustum(graphics.GetFrustum()))
	{
		return;
	}
	if (passType == PassType::GPass)
	{
		mainMaterial->BindDescriptorHeap(graphics.GetMainCommandList());
	}

	graphics.ExecuteBundle(drawingBundles[passType].Get());

	UpdateTransformBuffer(graphics);
	transformConstantBuffer->Bind(graphics.GetMainCommandList());
	graphics.GetMainCommandList()->DrawIndexedInstanced(mainModel->GetIndicesNumber(), 1, 0, 0, 0);
}

void MeshComponent::PrepareForPass(Graphics& graphics, Pass* const pass)
{
	SceneComponent::PrepareForPass(graphics, pass);

	auto passType = pass->GetType();

	switch (passType)
	{
	case PassType::GPass:
		PrepareForGPass(graphics, pass);
		break;
	case PassType::LightPerspectivePass:
		PrepareForLightPerspectivePass(graphics, pass);
		break;
	default:
		break;
	}
}

void MeshComponent::PrepareForGPass(Graphics& graphics, Pass* const pass)
{
	auto pss = pass->GetPSS();
	pss.inputLayout = mainModel->GetInputLayout();
	pss.vertexShader = CD3DX12_SHADER_BYTECODE(mainModel->GetVSBlob());
	pss.pixelShader = CD3DX12_SHADER_BYTECODE(mainMaterial->GetPSBlob());
	pss.rasterizer = mainMaterial->GetRasterizerDesc();

	mainPipelineState = std::make_unique<PipelineState>(graphics, pss);

	auto drawingBundle = graphics.CreateBundle();
	drawingBundle->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	mainPipelineState->Bind(drawingBundle.Get());
	pass->BindPassSpecific(drawingBundle.Get());

	mainModel->Bind(drawingBundle.Get());

	mainMaterial->Bind(graphics, drawingBundle.Get());

	CHECK_HR(drawingBundle->Close());
	drawingBundles[pass->GetType()] = std::move(drawingBundle);
}

void MeshComponent::PrepareForLightPerspectivePass(Graphics& graphics, Pass* const pass)
{
	auto drawingBundle = graphics.CreateBundle();
	drawingBundle->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pass->BindPassSpecific(drawingBundle.Get());
	primitiveModel->Bind(drawingBundle.Get());

	CHECK_HR(drawingBundle->Close());
	drawingBundles[pass->GetType()] = std::move(drawingBundle);
}

bool MeshComponent::IsInsideFrustum(const Graphics::Frustum& frustum) const
{
	using namespace DirectX;

	for (const auto& plane : frustum.planes) {
		XMVECTOR normal = XMVectorSet(plane.x, plane.y, plane.z, 0.0f);
		float distance = XMVectorGetX(XMVector3Dot(normal, XMLoadFloat3(&worldBoundingSphere.center))) + plane.w;

		if (distance < -worldBoundingSphere.radius)
			return false;
	}
	return true;
}

void MeshComponent::Update(Graphics& graphics)
{
	SceneComponent::Update(graphics);
	mainMaterial->Update();
}

Material* MeshComponent::GetMaterial() noexcept
{
	return mainMaterial.get();
}

void MeshComponent::UpdateTransformBuffer(Graphics& graphics)
{
	DirectX::XMMATRIX transform = DirectX::XMMatrixTranspose(GetTransformMatrix());
	DirectX::XMMATRIX view = DirectX::XMMatrixTranspose(graphics.GetCamera());
	DirectX::XMMATRIX projection = DirectX::XMMatrixTranspose(graphics.GetProjection());
	transformBuffer = TransformBuffer(std::move(transform), std::move(view), std::move(projection));
}

ShaderSettings MeshComponent::ResolveShaderSettings(const aiMesh* const mesh, const aiMaterial* const material)
{
	ShaderSettings resolvedSettings = {};

	aiString texFileName;
	material->GetTexture(aiTextureType_DIFFUSE, 0, &texFileName);
	aiString normalTexFileName;
	material->GetTexture(aiTextureType_NORMALS, 0, &normalTexFileName);
	aiString specularTexFileName;
	material->GetTexture(aiTextureType_SPECULAR, 0, &specularTexFileName);

	if (mesh->HasTextureCoords(0))
	{
		if (texFileName.length > 0)
		{
			resolvedSettings |= ShaderSettings::Texture;
		}
		else
		{
			resolvedSettings |= ShaderSettings::Color;
		}
		if (normalTexFileName.length > 0 && mesh->HasNormals() && mesh->HasTangentsAndBitangents())
		{
			resolvedSettings |= ShaderSettings::NormalMap;
		}
		if (specularTexFileName.length > 0)
		{
			resolvedSettings |= ShaderSettings::SpecularMap;
		}
	}
	else
	{
		resolvedSettings |= ShaderSettings::Color;
	}

	return resolvedSettings;
}

MeshComponent::BoundingSphere MeshComponent::CalculateModelBoundingSphere(const aiVector3D* const vertices, unsigned int verticesNum) const
{
	if (verticesNum == 0)
	{
		return {};
	}
	aiVector3D min = vertices[0];
	aiVector3D max = vertices[0];

	for (unsigned int i = 0; i < verticesNum; ++i)
	{
		min.x = std::min(min.x, vertices[i].x);
		min.y = std::min(min.y, vertices[i].y);
		min.z = std::min(min.z, vertices[i].z);
		max.x = std::max(max.x, vertices[i].x);
		max.y = std::max(max.y, vertices[i].y);
		max.z = std::max(max.z, vertices[i].z);
	}

	aiVector3D center = (min + max) * 0.5f;

	float radius = 0.0f;
	for (unsigned int i = 0; i < verticesNum; ++i)
	{
		float dist = (vertices[i] - center).Length();
		if (dist > radius)
		{
			radius = dist;
		}
	}

	return { {center.x, center.y, center.z}, radius };
}

void MeshComponent::UpdateWorldBoundingSphere()
{
	using namespace DirectX;
	const XMVECTOR worldScale = GetComponentScaleVector();
	const XMVECTOR worldLocation = GetComponentLocationVector();
	XMVECTOR center = XMLoadFloat3(&modelBoundingSphere.center);
	const XMMATRIX transform = GetTransformMatrix();
	center = XMVector3Transform(center, transform);
	XMStoreFloat3(&worldBoundingSphere.center, center);
	worldBoundingSphere.radius = modelBoundingSphere.radius * std::max({ XMVectorGetX(worldScale),
																	XMVectorGetY(worldScale),
																	XMVectorGetZ(worldScale) });
}

MeshComponent::TransformBuffer::TransformBuffer(const DirectX::XMMATRIX newTransform, const DirectX::XMMATRIX newView, const DirectX::XMMATRIX newProjection)
{
	DirectX::XMStoreFloat4x4(&transform, newTransform);
	DirectX::XMStoreFloat4x4(&view, newView);
	DirectX::XMStoreFloat4x4(&projection, newProjection);
}