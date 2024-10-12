#include "Material.h"
#include <assimp\material.h>
#include <d3d12.h>
#include <stdexcept>
#include "ThrowMacros.h"
#include <d3dcompiler.h>
#include "ConstantBuffer.h"
#include "Texture.h"
#include "Graphics.h"
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

const std::unordered_map<ShaderSettings, UINT, ShaderSettingsHash> Material::texturesNumMap =
{
	{ ShaderSettings::Skybox, 0u },
	{ ShaderSettings::Color, 0u },
	{ ShaderSettings::Color | ShaderSettings::Phong, 0u },
	{ ShaderSettings::Phong | ShaderSettings::Texture, 1u },
	{ ShaderSettings::Phong | ShaderSettings::Texture | ShaderSettings::NormalMap, 2u },
	{ ShaderSettings::Phong | ShaderSettings::Texture | ShaderSettings::SpecularMap, 2u },
	{ ShaderSettings::Phong | ShaderSettings::Texture | ShaderSettings::NormalMap | ShaderSettings::SpecularMap, 3u },
	{ ShaderSettings::Phong | ShaderSettings::Texture | ShaderSettings::NormalMap | ShaderSettings::SpecularMap | ShaderSettings::AlphaTesting, 3u },
};

Material::Material(Graphics& graphics, PipelineState::PipelineStateStream& pipelineStateStream, const aiMaterial* const assignedMaterial,
	ShaderSettings shaderSettings, std::vector<CD3DX12_ROOT_PARAMETER>& rootParameters)
{
	CD3DX12_RASTERIZER_DESC rasterizerDesc(D3D12_DEFAULT);

	const UINT texturesNum = texturesNumMap.at(shaderSettings);
	// descriptor heap for the shader resource view
	if (texturesNum > 0)
	{
		const D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc{
			.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
			.NumDescriptors = texturesNum,
			.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
		};
		CHECK_HR(graphics.GetDevice()->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&srvHeap)));

		srvCpuHandle = { srvHeap->GetCPUDescriptorHandleForHeapStart() };
	}

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
		auto diffTex = std::make_unique<Texture>(graphics, 1u, texFileName.C_Str(), srvCpuHandle);
		srvCpuHandle.Offset(graphics.GetCbvSrvDescriptorSize());
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
		textures.push_back(std::make_unique<Texture>(graphics, 2u, normalTexFileName.C_Str(), srvCpuHandle));
		srvCpuHandle.Offset(graphics.GetCbvSrvDescriptorSize());
	}
	if (static_cast<bool>(shaderSettings & ShaderSettings::SpecularMap))
	{
		aiString specularTexFileName;
		assignedMaterial->GetTexture(aiTextureType_SPECULAR, 0, &specularTexFileName);
		textures.push_back(std::make_unique<Texture>(graphics, 3u, specularTexFileName.C_Str(), srvCpuHandle));
		srvCpuHandle.Offset(graphics.GetCbvSrvDescriptorSize());
	}
	if (static_cast<bool>(shaderSettings & ShaderSettings::Phong))
	{
		roughnessBuffer = std::make_unique<Roughness>();
		cBuffers.push_back(std::make_unique<ConstantBuffer<Roughness>>(graphics, *roughnessBuffer, BufferType::Pixel, 1u, rootParameters));
	}

	if (!static_cast<bool>(shaderSettings & (ShaderSettings::Texture | ShaderSettings::Skybox)))
	{
		colorBuffer = std::make_unique<Color>();
		cBuffers.push_back(std::make_unique<ConstantBuffer<Color>>(graphics, *colorBuffer, BufferType::Pixel, 2u, rootParameters));
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

void Material::Bind(Graphics& graphics, ID3D12GraphicsCommandList* const commandList) noexcept
{
	for (auto& cBuffer : cBuffers)
	{
		cBuffer->Bind(commandList);
	}

	if (texesDescRanges.size() > 0)
	{
		commandList->SetDescriptorHeaps(1u, srvHeap.GetAddressOf());
		commandList->SetGraphicsRootDescriptorTable(desciptorTableRootIndex, srvHeap->GetGPUDescriptorHandleForHeapStart());
	}
}

void Material::Update()
{
	for (auto& cBuffer : cBuffers)
	{
		cBuffer->Update();
	}
}