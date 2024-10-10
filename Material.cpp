#include "Material.h"
#include <assimp\material.h>
#include "BindablesPool.h"
#include <d3d12.h>
#include <stdexcept>
#include "ThrowMacros.h"
#include <d3dcompiler.h>
#include "ConstantBuffer.h"
#include "Texture.h"
//#include "Sampler.h"
//#include "Blender.h"
//#include "CubeTexture.h"

const std::unordered_map<ShaderSettings, std::wstring, ShaderSettingsHash> Material::psPaths =
{
	{ ShaderSettings::Skybox, L"SkyboxPS.cso" },
	{ ShaderSettings::Color, L"SolidPS.cso" },
	{ ShaderSettings::Color | ShaderSettings::Phong, L"PhongColorPS.cso" },
	{ ShaderSettings::Phong | ShaderSettings::Texture, L"PhongTexPS.cso" },
	{ ShaderSettings::Phong | ShaderSettings::Texture | ShaderSettings::NormalMap, L"PhongTexNMPS.cso" },
	{ ShaderSettings::Phong | ShaderSettings::Texture | ShaderSettings::SpecularMap, L"PhongTexSMPS.cso" },
	{ ShaderSettings::Phong | ShaderSettings::Texture | ShaderSettings::NormalMap | ShaderSettings::SpecularMap, L"PhongTexNMSMPS.cso" },
	{ ShaderSettings::Phong | ShaderSettings::Texture | ShaderSettings::NormalMap | ShaderSettings::SpecularMap | ShaderSettings::AlphaTesting , L"PhongTexNMSMATPS.cso" },
};

Material::Material(Graphics& graphics, PipelineState::PipelineStateStream& pipelineStateStream, const aiMaterial* const assignedMaterial,
	ShaderSettings shaderSettings, std::vector<CD3DX12_ROOT_PARAMETER>& rootParameters)
{
	CD3DX12_RASTERIZER_DESC rasterizerDesc(D3D12_DEFAULT);

	auto& bindablesPool = BindablesPool::GetInstance();
	/*
	if (static_cast<bool>(shaderSettings & ShaderSettings::Skybox))
	{
		aiString texesPath;
		assignedMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &texesPath);
		sharedBindables.push_back(bindablesPool.GetBindable<CubeTexture>(graphics, 0u, texesPath.C_Str()));

		rsDesc.CullMode = D3D12_CULL_FRONT;
	}*/
	if (static_cast<bool>(shaderSettings & ShaderSettings::Texture))
	{
		aiString texFileName;
		assignedMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &texFileName);
		//sharedBindables.push_back(bindablesPool.GetBindable<Texture>(graphics, 1u, texFileName.C_Str(), rootParameters));
		auto diffTex = std::make_shared<Texture>(graphics, 1u, texFileName.C_Str(), rootParameters);
		if (diffTex->HasAlpha())
		{
			rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
			shaderSettings |= ShaderSettings::AlphaTesting;
		}
		textures.push_back(std::move(diffTex));
	}
	if (static_cast<bool>(shaderSettings & ShaderSettings::NormalMap))
	{
		aiString normalTexFileName;
		assignedMaterial->GetTexture(aiTextureType_NORMALS, 0, &normalTexFileName);
		//sharedBindables.push_back(bindablesPool.GetBindable<Texture>(graphics, 2u, normalTexFileName.C_Str(), rootParameters));
		textures.push_back(std::make_shared<Texture>(graphics, 2u, normalTexFileName.C_Str(), rootParameters));
	}
	if (static_cast<bool>(shaderSettings & ShaderSettings::SpecularMap))
	{
		aiString specularTexFileName;
		assignedMaterial->GetTexture(aiTextureType_SPECULAR, 0, &specularTexFileName);
		//sharedBindables.push_back(bindablesPool.GetBindable<Texture>(graphics, 3u, specularTexFileName.C_Str()));
		textures.push_back(std::make_shared<Texture>(graphics, 3u, specularTexFileName.C_Str(), rootParameters));
	}
	if (static_cast<bool>(shaderSettings & ShaderSettings::Phong))
	{
		roughnessBuffer = std::make_unique<Roughness>();
		bindables.push_back(std::make_unique<ConstantBuffer<Roughness>>(graphics, *roughnessBuffer, BufferType::Pixel, 1u, rootParameters));
	}

	if (!static_cast<bool>(shaderSettings & (ShaderSettings::Texture | ShaderSettings::Skybox)))
	{
		colorBuffer = std::make_unique<Color>();
		bindables.push_back(std::make_unique<ConstantBuffer<Color>>(graphics, *colorBuffer, BufferType::Pixel, 2u, rootParameters));
	}
	/*
	if (static_cast<bool>(shaderSettings & (ShaderSettings::Texture | ShaderSettings::NormalMap | ShaderSettings::SpecularMap)))
	{
		//sharedBindables.push_back(bindablesPool.GetBindable<Sampler>(graphics, 1u, Sampler::Mode::Anisotropic));
	}
	if (static_cast<bool>(shaderSettings & (ShaderSettings::Skybox)))
	{
		sharedBindables.push_back(bindablesPool.GetBindable<Sampler>(graphics, 1u, Sampler::Mode::Biliniear));
	}*/

	if (textures.size() > 0)
	{
		CD3DX12_ROOT_PARAMETER rootParameter{};
		texesDescRanges = {};
		UINT i = 0;
		for (const auto& tex : textures)
		{
			texesDescRanges.push_back(D3D12_DESCRIPTOR_RANGE{ D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, tex->GetSlot(), 0u, i });
			i++;
		}
		rootParameter.InitAsDescriptorTable((UINT)texesDescRanges.size(), texesDescRanges.data(), D3D12_SHADER_VISIBILITY_PIXEL);
		rootParameters.push_back(std::move(rootParameter));
		desciptorTableRootIndex = (UINT)rootParameters.size() - 1;
	}

	std::wstring pixelShaderPath;

	auto it = Material::psPaths.find(shaderSettings);
	if (it != Material::psPaths.end())
	{
		pixelShaderPath = it->second;
	}
	else
	{
		throw std::runtime_error("Pixel shader path not found for given flags");
	}

	// Load the pixel shader.
	CHECK_HR(D3DReadFileToBlob(pixelShaderPath.c_str(), &pixelShaderBlob));

	pipelineStateStream.pixelShader = CD3DX12_SHADER_BYTECODE(pixelShaderBlob.Get());
	pipelineStateStream.rasterizer = rasterizerDesc;
}

void Material::Bind(Graphics& graphics) noexcept
{
	for (auto& bindable : bindables)
	{
		bindable->Update(graphics);
		bindable->Bind(graphics);
	}

	if (texesDescRanges.size() > 0)
	{
		auto srvGpuHandle = graphics.GetCbvSrvGpuHeapStartHandle();
		graphics.commandList->SetGraphicsRootDescriptorTable(desciptorTableRootIndex, srvGpuHandle);
	}
}