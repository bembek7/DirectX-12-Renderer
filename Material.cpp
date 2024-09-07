#include "Material.h"
#include <assimp\material.h>
#include "ConstantBuffer.h"
#include "PixelShader.h"
#include "BindablesPool.h"
#include "Texture.h"
#include "Sampler.h"

Material::Material(Graphics& graphics, const aiMaterial* const assignedMaterial, const bool usesPhong)
{
	std::wstring pixelShaderPath = L"SolidPS.cso";
	auto& bindablesPool = BindablesPool::GetInstance();

	if (usesPhong)
	{
		aiString texFileName;
		assignedMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &texFileName);

		if (texFileName.length > 0)
		{
			sharedBindables.push_back(bindablesPool.GetBindable<Texture>(graphics, 1u, texFileName.C_Str()));
			pixelShaderPath = L"PhongTexPS.cso";

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
			bindables.push_back(std::make_unique<ConstantBuffer<Roughness>>(graphics, roughnessBuffer, BufferType::Pixel, 1u));
		}
		else
		{
			colorBuffer = std::make_unique<Color>();
			bindables.push_back(std::make_unique<ConstantBuffer<Color>>(graphics, *colorBuffer, BufferType::Pixel, 2u));
			pixelShaderPath = L"PhongColorPS.cso";
		}
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