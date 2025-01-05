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
#include "RegularDrawingPass.h"
#include "ShadowMappingPass.h"
#include "DepthPrePass.h"

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

	lighted = static_cast<bool>(shaderSettings & ShaderSettings::Phong);

	transformConstantBuffer = std::make_unique<ConstantBufferConstants<TransformBuffer>>(transformBuffer, RPD::Transform);

	primitiveModel = std::make_unique<Model>(graphics, assignedMesh, ShaderSettings{});

	PipelineState::PipelineStateStream pipelineStateStream = RegularDrawingPass::GetCommonPSS();

	model = std::make_unique<Model>(graphics, assignedMesh, shaderSettings, nullptr);
	pipelineStateStream.inputLayout = model->GetInputLayout();
	pipelineStateStream.vertexShader = CD3DX12_SHADER_BYTECODE(model->GetVSBlob());
	material = std::make_unique<Material>(graphics, pipelineStateStream, assignedMaterial, shaderSettings);

	pipelineStateStream.rootSignature = graphics.GetRootSignature()->Get();

	auto& pipelineStatesPool = PipelineStatesPool::GetInstance();
	pipelineState = pipelineStatesPool.GetPipelineState(graphics, shaderSettings, pipelineStateStream);

	// Create and record drawing bundle.
	drawingBundle = graphics.CreateBundle();
	drawingBundle->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pipelineState->Bind(drawingBundle.Get());
	graphics.GetRootSignature()->Bind(drawingBundle.Get());

	model->Bind(drawingBundle.Get());
	material->Bind(graphics, drawingBundle.Get());
	CHECK_HR(drawingBundle->Close());

	if (lighted)
	{
		static constexpr auto smShaderSettings = ShaderSettings::ShadowMapping;

		auto smPipelineStateStream = ShadowMappingPass::GetCommonPSS();
		modelForShadowMapping = std::make_unique<Model>(graphics, assignedMesh, smShaderSettings, model->ShareIndexBuffer());
		smPipelineStateStream.rootSignature = graphics.GetRootSignature()->Get();
		smPipelineStateStream.inputLayout = modelForShadowMapping->GetInputLayout();
		smPipelineStateStream.vertexShader = CD3DX12_SHADER_BYTECODE(modelForShadowMapping->GetVSBlob());
		smPipelineState = pipelineStatesPool.GetPipelineState(graphics, smShaderSettings, smPipelineStateStream);

		// Create and record shadow map bundle.
		shadowMappingBundle = graphics.CreateBundle();
		shadowMappingBundle->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		smPipelineState->Bind(shadowMappingBundle.Get());
		graphics.GetRootSignature()->Bind(shadowMappingBundle.Get());

		modelForShadowMapping->Bind(shadowMappingBundle.Get());
		CHECK_HR(shadowMappingBundle->Close());
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

void MeshComponent::Draw(Graphics& graphics, const std::vector<Light*>& lights)
{
	SceneComponent::Draw(graphics, lights);
	material->BindDescriptorHeap(graphics.GetMainCommandList());
	graphics.ExecuteBundle(drawingBundle.Get());
	UpdateTransformBuffer(graphics);
	transformConstantBuffer->Bind(graphics.GetMainCommandList());
	if (lighted)
	{
		for (auto& light : lights)
		{
			light->Bind(graphics.GetMainCommandList());
		}
	}

	graphics.GetMainCommandList()->DrawIndexedInstanced(model->GetIndicesNumber(), 1, 0, 0, 0);
}

void MeshComponent::Draw(Graphics& graphics, const PassType& passType)
{
	SceneComponent::Draw(graphics, passType);
	graphics.ExecuteBundle(passSpecificSettings[passType].drawingBundle.Get());
	UpdateTransformBuffer(graphics);
	transformConstantBuffer->Bind(graphics.GetMainCommandList());
	graphics.GetMainCommandList()->DrawIndexedInstanced(passSpecificSettings[passType].model->GetIndicesNumber(), 1, 0, 0, 0);
}

void MeshComponent::PrepareForPass(Graphics& graphics, const Pass* const pass)
{
	SceneComponent::PrepareForPass(graphics, pass);
	auto pss = pass->GetPSS();

	Model* model;

	switch (pass->GetType())
	{
	default:
		model = primitiveModel.get();
		break;
	}
	
	pss.inputLayout = model->GetInputLayout();
	pss.vertexShader = CD3DX12_SHADER_BYTECODE(model->GetVSBlob());

	auto& pipelineStatesPool = PipelineStatesPool::GetInstance();
	auto ps = pipelineStatesPool.GetPipelineState(graphics, ShaderSettings::Color, pss);

	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> drawingBundle;
	drawingBundle = graphics.CreateBundle();
	drawingBundle->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	ps->Bind(drawingBundle.Get());
	graphics.GetRootSignature()->Bind(drawingBundle.Get());

	model->Bind(drawingBundle.Get());
	CHECK_HR(drawingBundle->Close());

	passSpecificSettings[pass->GetType()] = { std::move(ps), std::move(drawingBundle), model };
}

void MeshComponent::Update(Graphics& graphics)
{
	SceneComponent::Update(graphics);
	material->Update();
}

void MeshComponent::RenderShadowMap(Graphics& graphics)
{
	if (lighted)
	{
		graphics.ExecuteBundle(shadowMappingBundle.Get());
		UpdateTransformBuffer(graphics);
		transformConstantBuffer->Bind(graphics.GetMainCommandList());
		graphics.GetMainCommandList()->DrawIndexedInstanced(modelForShadowMapping->GetIndicesNumber(), 1, 0, 0, 0);
	}
}

Material* MeshComponent::GetMaterial() noexcept
{
	return material.get();
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

MeshComponent::TransformBuffer::TransformBuffer(const DirectX::XMMATRIX newTransform, const DirectX::XMMATRIX newView, const DirectX::XMMATRIX newProjection)
{
	DirectX::XMStoreFloat4x4(&transform, newTransform);
	DirectX::XMStoreFloat4x4(&view, newView);
	DirectX::XMStoreFloat4x4(&projection, newProjection);
}