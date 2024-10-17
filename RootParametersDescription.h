#pragma once
#include "d3d12.h"
#include <vector>

namespace RPD
{
	enum ParamsIndexes
	{
		Transform = 0u,
		ShadowMap = 1u,
		Light = 2u,
		Roughness = 3u,
		Color = 4u,
		TexturesDescTable = 5u // last of params
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
		{1u, D3D12_SHADER_VISIBILITY_VERTEX, ShadowMap},
		{0u, D3D12_SHADER_VISIBILITY_PIXEL, Light},
		{1u, D3D12_SHADER_VISIBILITY_PIXEL, Roughness},
		{2u, D3D12_SHADER_VISIBILITY_PIXEL, Color}
	};

	static constexpr UINT paramsNum = TexturesDescTable + 1;

	static constexpr UINT texturesNum = 4u;
}