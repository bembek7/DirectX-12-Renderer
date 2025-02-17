#pragma once

enum class ShaderSettings
{
	Phong = 1,
	Color = 2,
	Texture = 4,
	NormalMap = 8,
	SpecularMap = 16,
	AlphaTesting = 32,
	Skybox = 64,
	ShadowMapping = 128
};

struct ShaderSettingsHash
{
	template <typename T>
	std::size_t operator()(T t) const
	{
		return static_cast<std::size_t>(t);
	}
};

DEFINE_ENUM_FLAG_OPERATORS(ShaderSettings);