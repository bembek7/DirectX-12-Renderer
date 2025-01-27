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
		SceneNormal_Roughness,
		SceneSpecularColor,
		SceneViewPosition,
		LightMap,
		DepthBuffer,
		ShadowMap
	};

	enum class CBTypes
	{
		Transform,
		Roughness,
		Color,
		LightProperties,
	};

	enum class SamplerTypes
	{
		Anisotropic,
		Comparison
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
		D3D12_TEXTURE_ADDRESS_MODE addressMode;
		D3D12_STATIC_BORDER_COLOR borderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE;
		D3D12_COMPARISON_FUNC comparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	};
	
	static const std::unordered_map<TextureTypes, UINT, EnumClassesHash> texturesSlots =
	{
		// GPass
		{TextureTypes::Diffuse, 0u},
		{TextureTypes::NormalMap, 1u},
		{TextureTypes::SpecularMap, 2u},
		// FinalPass
		{TextureTypes::SceneColor, 0u},
		{TextureTypes::LightMap, 1u},
		{TextureTypes::ShadowMap, 2u},
		// LightPass
		{TextureTypes::SceneNormal_Roughness, 0u},
		{TextureTypes::SceneSpecularColor, 1u},
		{TextureTypes::SceneViewPosition, 2u},
		// ShadowPass
		{TextureTypes::DepthBuffer, 0u},
	};

	static const std::unordered_map<CBTypes, CBInfo, EnumClassesHash> cbsInfo =
	{
		// GPass
		{CBTypes::Transform, {D3D12_SHADER_VISIBILITY_VERTEX, 0u, 192u}},
		{CBTypes::Roughness, {D3D12_SHADER_VISIBILITY_PIXEL, 0u}},
		{CBTypes::Color, {D3D12_SHADER_VISIBILITY_PIXEL, 1u}},
		// LightPass
		{CBTypes::LightProperties, {D3D12_SHADER_VISIBILITY_PIXEL, 0u}},
		// ShadowPass
	};

	static const std::unordered_map<SamplerTypes, SamplerInfo, EnumClassesHash> samplersInfo =
	{
		{SamplerTypes::Anisotropic, {D3D12_SHADER_VISIBILITY_PIXEL, 1u, D3D12_FILTER_ANISOTROPIC, D3D12_REQ_MAXANISOTROPY, D3D12_TEXTURE_ADDRESS_MODE_WRAP}},
		{SamplerTypes::Comparison, {D3D12_SHADER_VISIBILITY_PIXEL, 0u, D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR, D3D12_REQ_MAXANISOTROPY, D3D12_TEXTURE_ADDRESS_MODE_BORDER, D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK, D3D12_COMPARISON_FUNC_LESS_EQUAL}},
	};
}