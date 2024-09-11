#include "Material.h"
#include <assimp\material.h>
#include "ConstantBuffer.h"
#include "PixelShader.h"
#include "BindablesPool.h"
#include "Texture.h"
#include "Sampler.h"
#include "Blender.h"
#include "Rasterizer.h"
#include "CubeTexture.h"

const std::unordered_map<ShaderSettings, std::wstring, ShaderSettingsHash> Material::psPaths =
{
	{ ShaderSettings{}, L"SolidPS.cso"},
	{ ShaderSettings::Skybox, L"SkyboxPS.cso" },
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

	auto cullMode = D3D11_CULL_BACK;

	if (static_cast<bool>(shaderSettings & ShaderSettings::Skybox))
	{
		aiString texesPath;
		assignedMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &texesPath);
		sharedBindables.push_back(bindablesPool.GetBindable<CubeTexture>(graphics, 0u, texesPath.C_Str()));

		cullMode = D3D11_CULL_FRONT;
	}
	if (static_cast<bool>(shaderSettings & ShaderSettings::Texture))
	{
		aiString texFileName;
		assignedMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &texFileName);
		sharedBindables.push_back(bindablesPool.GetBindable<Texture>(graphics, 1u, texFileName.C_Str()));

		auto diffTex = reinterpret_cast<Texture*>(sharedBindables.back().get());
		if (diffTex->HasAlpha())
		{
			cullMode = D3D11_CULL_NONE;
			shaderSettings |= ShaderSettings::AlphaTesting;
		}
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
		roughnessBuffer = std::make_unique<Roughness>();
		bindables.push_back(std::make_unique<ConstantBuffer<Roughness>>(graphics, *roughnessBuffer, BufferType::Pixel, 1u));
	}
	if (!static_cast<bool>(shaderSettings & (ShaderSettings::Texture | ShaderSettings::Skybox)))
	{
		colorBuffer = std::make_unique<Color>();
		bindables.push_back(std::make_unique<ConstantBuffer<Color>>(graphics, *colorBuffer, BufferType::Pixel, 2u));
		sharedBindables.push_back(bindablesPool.GetBindable<Blender>(graphics, false));
	}

	sharedBindables.push_back(bindablesPool.GetBindable<Rasterizer>(graphics, cullMode));

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
		sharedBindables.push_back(bindablesPool.GetBindable<Sampler>(graphics, 1u, Sampler::Mode::Anisotropic));
	}
	if (static_cast<bool>(shaderSettings & (ShaderSettings::Skybox)))
	{
		sharedBindables.push_back(bindablesPool.GetBindable<Sampler>(graphics, 1u, Sampler::Mode::Biliniear));
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