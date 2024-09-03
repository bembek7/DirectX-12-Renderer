#include "Material.h"
#include <assimp\material.h>
#include "ConstantBuffer.h"
#include "PixelShader.h"
#include "BindablesPool.h"

Material::Material(Graphics& graphics, const aiMaterial* const assignedMaterial)
{
	std::wstring pixelShaderPath;
	pixelShaderPath = L"PhongPS.cso";

	bindables.push_back(std::make_unique<ConstantBuffer<ColorBuffer>>(graphics, colorBuffer, BufferType::Pixel));

	bindables.push_back(std::make_unique<PixelShader>(graphics, pixelShaderPath));
	/*auto& bindablesPool = BindablesPool::GetInstance();
	sharedBindables.push_back(bindablesPool.GetBindable<PixelShader>(graphics, pixelShaderPath));*/
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