#pragma once
#include "d3d12.h"
#include <unordered_map>

namespace RPD
{
	enum class TextureTypes
	{
		Diffuse,
		NormalMap,
		SpecularMap,
	};

	enum class CBTypes
	{
		Transform,
		Roughness,
		Color,
	};

	namespace
	{
		struct EnumClassesHash
		{
			template <typename T>
			std::size_t operator()(T t) const
			{
				return static_cast<std::size_t>(t);
			}
		};
	}

	struct CBInfo
	{
		D3D12_SHADER_VISIBILITY visibility;
		UINT slot;
		UINT size = 0u;
	};
	
	static const std::unordered_map<TextureTypes, UINT, EnumClassesHash> texturesSlots =
	{
		{TextureTypes::Diffuse, 0u},
		{TextureTypes::NormalMap, 1u},
		{TextureTypes::SpecularMap, 2u},
	};

	static const std::unordered_map<CBTypes, CBInfo, EnumClassesHash> cbsInfo =
	{
		{CBTypes::Transform, {D3D12_SHADER_VISIBILITY_VERTEX, 0u, 192u}},
		{CBTypes::Roughness, {D3D12_SHADER_VISIBILITY_PIXEL, 0u}},
		{CBTypes::Color, {D3D12_SHADER_VISIBILITY_PIXEL, 1u}},
	};

}