#pragma once
#include "d3d12.h"
#include <vector>

namespace RPD
{
	enum ParamsIndexes
	{
		Transform = 0u,
		ShadowMapping = Transform + 1u,
		Roughness = ShadowMapping + 1u,
		Color = Roughness + 1u,
		DirectionalLight = Color + 1u,
		PointLight = DirectionalLight + 1u,
		SpotLight = PointLight + 1u,
		TexturesDescTable = SpotLight + 1u // last of params
	};

	enum TextureSlots
	{
		Diffuse = 1u,
		NormalMap = 2u,
		SpecularMap = 3u,
	};

	struct CBV
	{
		UINT slot;
		D3D12_SHADER_VISIBILITY visibility;
		ParamsIndexes ParamIndex;
	};

	static const std::vector<CBV> cbvs =
	{
		{0u, D3D12_SHADER_VISIBILITY_VERTEX, Transform},
		{1u, D3D12_SHADER_VISIBILITY_VERTEX, ShadowMapping},
		{0u, D3D12_SHADER_VISIBILITY_PIXEL, Roughness},
		{1u, D3D12_SHADER_VISIBILITY_PIXEL, Color},
		{2u, D3D12_SHADER_VISIBILITY_PIXEL, DirectionalLight},
		{3u, D3D12_SHADER_VISIBILITY_PIXEL, PointLight},
		{4u, D3D12_SHADER_VISIBILITY_PIXEL, SpotLight},
	};

	static constexpr UINT paramsNum = TexturesDescTable + 1;

	static constexpr UINT texturesNum = 4u;
}