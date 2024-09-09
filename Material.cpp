#include "Material.h"
#include <assimp\material.h>
#include "ConstantBuffer.h"
#include "PixelShader.h"
#include "BindablesPool.h"
#include "Texture.h"
#include "Sampler.h"
#include "Blender.h"
#include "Rasterizer.h"

const std::unordered_map<ShaderSettings, std::wstring, ShaderSettingsHash> Material::psPaths =
{
	{ ShaderSettings::Color, L"SolidPS.cso" },
	{ ShaderSettings::Color | ShaderSettings::Phong, L"PhongColorPS.cso" },
	{ ShaderSettings::Phong | ShaderSettings::Texture, L"PhongTexPS.cso" },
	{ ShaderSettings::Phong | ShaderSettings::Texture | ShaderSettings::NormalMap, L"PhongTexNMPS.cso" },
	{ ShaderSettings::Phong | ShaderSettings::Texture | ShaderSettings::SpecularMap, L"PhongTexSMPS.cso" },
	{ ShaderSettings::Phong | ShaderSettings::Texture | ShaderSettings::NormalMap | ShaderSettings::SpecularMap, L"PhongTexNMSMPS.cso" },
	{ ShaderSettings::Phong | ShaderSettings::Texture | ShaderSettings::NormalMap | ShaderSettings::SpecularMap | ShaderSettings::AlphaTesting , L"PhongTexNMSMATPS.cso" },
};

Material::Material(Graphics& graphics, const aiMaterial* const assignedMaterial, ShaderSettings shaderSettings)
{
	auto& bindablesPool = BindablesPool::GetInstance();

	bool usesAlphaTesting = false;

	if (static_cast<bool>(shaderSettings & ShaderSettings::Texture))
	{
		aiString texFileName;
		assignedMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &texFileName);
		sharedBindables.push_back(bindablesPool.GetBindable<Texture>(graphics, 1u, texFileName.C_Str()));

		auto diffTex = reinterpret_cast<Texture*>(sharedBindables.back().get());
		usesAlphaTesting = diffTex->HasAlpha();
	}
	if (static_cast<bool>(shaderSettings & ShaderSettings::NormalMap))
	{
		aiString normalTexFileName;
		assignedMaterial->GetTexture(aiTextureType_NORMALS, 0, &normalTexFileName);
		sharedBindables.push_back(bindablesPool.GetBindable<Texture>(graphics, 2u, normalTexFileName.C_Str()));
	}
	if (static_cast<bool>(shaderSettings & ShaderSettings::SpecularMap))
	{
		aiString specularTexFileName;
		assignedMaterial->GetTexture(aiTextureType_SPECULAR, 0, &specularTexFileName);
		sharedBindables.push_back(bindablesPool.GetBindable<Texture>(graphics, 3u, specularTexFileName.C_Str()));
	}
	if (static_cast<bool>(shaderSettings & ShaderSettings::Phong))
	{
		bindables.push_back(std::make_unique<ConstantBuffer<Roughness>>(graphics, roughnessBuffer, BufferType::Pixel, 1u));
	}
	if (!static_cast<bool>(shaderSettings & ShaderSettings::Texture))
	{
		colorBuffer = std::make_unique<Color>();
		bindables.push_back(std::make_unique<ConstantBuffer<Color>>(graphics, *colorBuffer, BufferType::Pixel, 2u));
		sharedBindables.push_back(bindablesPool.GetBindable<Blender>(graphics, false));
	}

	if (usesAlphaTesting)
	{
		sharedBindables.push_back(bindablesPool.GetBindable<Rasterizer>(graphics, D3D11_CULL_NONE));
		shaderSettings |= ShaderSettings::AlphaTesting;
	}
	else
	{
		sharedBindables.push_back(bindablesPool.GetBindable<Rasterizer>(graphics, D3D11_CULL_BACK));
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

	if (static_cast<bool>(shaderSettings & (ShaderSettings::Texture | ShaderSettings::NormalMap | ShaderSettings::SpecularMap)))
	{
		D3D11_SAMPLER_DESC samplerDesc = {};
		samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.MaxAnisotropy = D3D11_REQ_MAXANISOTROPY;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		samplerDesc.MipLODBias = 0.0f;
		samplerDesc.MinLOD = 0.0f;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

		bindables.push_back(std::make_unique<Sampler>(graphics, 1u, samplerDesc));
	}

	sharedBindables.push_back(bindablesPool.GetBindable<PixelShader>(graphics, pixelShaderPath));
}

void Material::Bind(Graphics& graphics) noexcept
{
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
}