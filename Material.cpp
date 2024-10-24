#include "Material.h"
#include <assimp\material.h>
#include <d3d12.h>
#include <stdexcept>
#include "ThrowMacros.h"
#include "ShadersPool.h"
#include "ConstantBuffer.h"
#include "Texture.h"
#include "Graphics.h"
#include "RootParametersDescription.h"

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

const std::unordered_map<ShaderSettings, INT, ShaderSettingsHash> Material::textureHighestSlotMap =
{
	{ ShaderSettings::Skybox, -1 },
	{ ShaderSettings::Color, -1 },
	{ ShaderSettings::Color | ShaderSettings::Phong, -1 },
	{ ShaderSettings::Phong | ShaderSettings::Texture, 1 },
	{ ShaderSettings::Phong | ShaderSettings::Texture | ShaderSettings::NormalMap, 2 },
	{ ShaderSettings::Phong | ShaderSettings::Texture | ShaderSettings::SpecularMap, 3 },
	{ ShaderSettings::Phong | ShaderSettings::Texture | ShaderSettings::NormalMap | ShaderSettings::SpecularMap, 3 },
	{ ShaderSettings::Phong | ShaderSettings::Texture | ShaderSettings::NormalMap | ShaderSettings::SpecularMap | ShaderSettings::AlphaTesting, 3 },
};

Material::Material(Graphics& graphics, PipelineState::PipelineStateStream& pipelineStateStream, const aiMaterial* const assignedMaterial,
	ShaderSettings& shaderSettings)
{
	auto rasterizerDesc = CD3DX12_RASTERIZER_DESC(CD3DX12_DEFAULT{});
	const UINT descriptorsNum = textureHighestSlotMap.at(shaderSettings) + 1;
	// descriptor heap for the shader resource view
	CD3DX12_CPU_DESCRIPTOR_HANDLE srvCpuStartHandle{};
	if (descriptorsNum > 0)
	{
		const D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc{
			.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
			.NumDescriptors = descriptorsNum,
			.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
		};
		CHECK_HR(graphics.GetDevice()->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&srvHeap)));

		srvCpuStartHandle = { srvHeap->GetCPUDescriptorHandleForHeapStart() };
	}

	/*
	if (static_cast<bool>(shaderSettings & ShaderSettings::Skybox))
	{
		aiString texesPath;
		assignedMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &texesPath);
		sharedBindables.push_back(bindablesPool.GetBindable<CubeTexture>(graphics, 0u, texesPath.C_Str()));

		rsDesc.CullMode = D3D12_CULL_FRONT;
	}*/
	const auto srvDescSize = graphics.GetCbvSrvDescriptorSize();
	CD3DX12_CPU_DESCRIPTOR_HANDLE srvCpuHandle{};

	if (static_cast<bool>(shaderSettings & ShaderSettings::Texture))
	{
		aiString texFileName;
		assignedMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &texFileName);
		srvCpuHandle.InitOffsetted(srvCpuStartHandle, RPD::Diffuse, srvDescSize);
		auto diffTex = std::make_unique<Texture>(graphics, texFileName.C_Str(), srvCpuHandle);
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
		srvCpuHandle.InitOffsetted(srvCpuStartHandle, RPD::NormalMap, srvDescSize);
		textures.push_back(std::make_unique<Texture>(graphics, normalTexFileName.C_Str(), srvCpuHandle));
	}
	if (static_cast<bool>(shaderSettings & ShaderSettings::SpecularMap))
	{
		aiString specularTexFileName;
		assignedMaterial->GetTexture(aiTextureType_SPECULAR, 0, &specularTexFileName);
		srvCpuHandle.InitOffsetted(srvCpuStartHandle, RPD::SpecularMap, srvDescSize);
		textures.push_back(std::make_unique<Texture>(graphics, specularTexFileName.C_Str(), srvCpuHandle));
	}
	if (static_cast<bool>(shaderSettings & ShaderSettings::Phong))
	{
		roughnessBuffer = std::make_unique<Roughness>();
		cBuffers.push_back(std::make_unique<ConstantBufferCBV<Roughness>>(graphics, *roughnessBuffer, RPD::Roughness));
	}

	if (!static_cast<bool>(shaderSettings & (ShaderSettings::Texture | ShaderSettings::Skybox)))
	{
		colorBuffer = std::make_unique<Color>();
		cBuffers.push_back(std::make_unique<ConstantBufferCBV<Color>>(graphics, *colorBuffer, RPD::Color));
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

	auto& shadersPool = ShadersPool::GetInstance();
	pixelShaderBlob = shadersPool.GetShaderBlob(pixelShaderPath); // be wary that its a shared ptr to com ptr

	pipelineStateStream.pixelShader = CD3DX12_SHADER_BYTECODE(pixelShaderBlob->Get());
	pipelineStateStream.rasterizer = rasterizerDesc;
}

void Material::Bind(Graphics& graphics, ID3D12GraphicsCommandList* const commandList) noexcept
{
	for (auto& cBuffer : cBuffers)
	{
		cBuffer->Bind(commandList);
	}

	if (srvHeap)
	{
		BindDescriptorHeap(commandList);
		commandList->SetGraphicsRootDescriptorTable(RPD::TexturesDescTable, srvHeap->GetGPUDescriptorHandleForHeapStart());
	}
}

void Material::BindDescriptorHeap(ID3D12GraphicsCommandList* const commandList) noexcept
{
	if (srvHeap)
	{
		commandList->SetDescriptorHeaps(1u, srvHeap.GetAddressOf());
	}
}

void Material::Update()
{
	for (auto& cBuffer : cBuffers)
	{
		cBuffer->Update();
	}
}