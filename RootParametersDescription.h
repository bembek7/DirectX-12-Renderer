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
		SceneColor,
	};

	enum class CBTypes
	{
		Transform,
		Roughness,
		Color,
	};

	enum class SamplerTypes
	{
		Anisotropic,
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

	struct SamplerInfo
	{
		D3D12_SHADER_VISIBILITY visibility;
		UINT slot;
		D3D12_FILTER filter;
		UINT maxAnisotropy;
	};
	
	static const std::unordered_map<TextureTypes, UINT, EnumClassesHash> texturesSlots =
	{
		// GPass
		{TextureTypes::Diffuse, 0u},
		{TextureTypes::NormalMap, 1u},
		{TextureTypes::SpecularMap, 2u},
		// FinalPass
		{TextureTypes::SceneColor, 0u},
	};

	static const std::unordered_map<CBTypes, CBInfo, EnumClassesHash> cbsInfo =
	{
		{CBTypes::Transform, {D3D12_SHADER_VISIBILITY_VERTEX, 0u, 192u}},
		{CBTypes::Roughness, {D3D12_SHADER_VISIBILITY_PIXEL, 0u}},
		{CBTypes::Color, {D3D12_SHADER_VISIBILITY_PIXEL, 1u}},
	};

	static const std::unordered_map<SamplerTypes, SamplerInfo, EnumClassesHash> samplersInfo =
	{
		{SamplerTypes::Anisotropic, {D3D12_SHADER_VISIBILITY_PIXEL, 1u, D3D12_FILTER_ANISOTROPIC, D3D12_REQ_MAXANISOTROPY}},
	};
}