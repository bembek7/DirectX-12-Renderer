#include "BetterWindows.h"
#include "MeshComponent.h"
#include "Graphics.h"
#include <stdexcept>
#include <string>
#include <cassert>
#include "Bindable.h"
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

	const ShaderSettings shaderSettings = ResolveShaderSettings(assignedMesh, assignedMaterial);

	lighted = static_cast<bool>(shaderSettings & ShaderSettings::Phong);

	std::vector<CD3DX12_ROOT_PARAMETER> rootParameters = graphics.GetCommonRootParametersRef();

	if (lighted && rootParameters.empty())
	{
		throw std::runtime_error("Light actor has to be created and bound before actors using it are created");
	}

	transformConstantBuffer = std::make_unique<ConstantBuffer<TransformBuffer>>(graphics, transformBuffer, BufferType::Vertex, 0u, rootParameters);

	PipelineState::PipelineStateStream pipelineStateStream;

	model = std::make_unique<Model>(graphics, pipelineStateStream, assignedMesh, shaderSettings, nullptr);
	material = std::make_unique<Material>(graphics, pipelineStateStream, assignedMaterial, shaderSettings, rootParameters);

	rootSignature = std::make_unique<RootSignature>(graphics, rootParameters);

	// filling pso structure
	pipelineStateStream.rootSignature = rootSignature->Get();
	pipelineStateStream.primitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	pipelineStateStream.renderTargetFormats = {
		.RTFormats{ graphics.GetRTFormat() },
		.NumRenderTargets = 1,
	};
	pipelineStateStream.dsvFormat = DXGI_FORMAT_D32_FLOAT;
	const CD3DX12_DEPTH_STENCIL_DESC depthStencilDesc(CD3DX12_DEFAULT{});

	pipelineStateStream.depthStencil = depthStencilDesc;

	if (lighted)
	{
		//modelForShadowMapping = std::make_unique<Model>(graphics, assignedMesh, ShaderSettings{}, model->ShareIndexBuffer());
	}

	pipelineState = std::make_unique<PipelineState>(graphics, pipelineStateStream);

	// Create and record the bundle.
	{
		bundle = graphics.CreateBundle();
		bundle->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		pipelineState->Bind(bundle.Get());
		rootSignature->Bind(bundle.Get());
		transformConstantBuffer->Bind(bundle.Get());
		model->Bind(bundle.Get());
		material->Bind(graphics, bundle.Get());
		if (lighted)
		{
			graphics.BindLighting(bundle.Get());
		}
		bundle.Get()->DrawIndexedInstanced(model->GetIndicesNumber(), 1, 0, 0, 0);
		CHECK_HR(bundle->Close());
	}
}

void MeshComponent::RenderComponentDetails(Gui& gui)
{
	SceneComponent::RenderComponentDetails(gui);
	gui.RenderComponentDetails(this);
}

std::unique_ptr<MeshComponent> MeshComponent::CreateComponent(Graphics& graphics, const aiNode* const node, const aiScene* const scene)
{
	return std::unique_ptr<MeshComponent>(new MeshComponent(graphics, node, scene));
}

void MeshComponent::Draw(Graphics& graphics)
{
	UpdateTransformBuffer(graphics);
	transformConstantBuffer->Update();
	material->Update();
	graphics.ExecuteBundle(bundle.Get());
}

void MeshComponent::RenderShadowMap(Graphics& graphics)
{
	/*if (generatesShadow)
	{
		UpdateTransformBuffer(graphics);
		modelForShadowMapping->Bind(graphics);

		transformConstantBuffer->Update(graphics);
		transformConstantBuffer->Bind(graphics);

		graphics.DrawIndexed(modelForShadowMapping->GetIndicesNumber());
	}*/
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
	transformBuffer = TransformBuffer(std::move(DirectX::XMMatrixTranspose(transformMatrix)), std::move(transformView), std::move(transformViewProjection));
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

	int shadingModel = 0;
	material->Get(AI_MATKEY_SHADING_MODEL, shadingModel);

	if (shadingModel == aiShadingMode_Phong)
	{
		resolvedSettings |= ShaderSettings::Phong;
	}
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

	return resolvedSettings;
}

MeshComponent::TransformBuffer::TransformBuffer(const DirectX::XMMATRIX newTransform, const DirectX::XMMATRIX newTransformView, const DirectX::XMMATRIX newTransformViewProjection)
{
	DirectX::XMStoreFloat4x4(&transform, newTransform);
	DirectX::XMStoreFloat4x4(&transformView, newTransformView);
	DirectX::XMStoreFloat4x4(&transformViewProjection, newTransformViewProjection);
}