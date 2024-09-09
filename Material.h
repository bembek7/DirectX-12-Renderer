#pragma once
#include "BetterWindows.h"
#include "Bindable.h"
#include <vector>
#include <memory>
#include <DirectXMath.h>
#include <string>
#include <unordered_map>
#include "ShaderSettings.h"

struct aiMaterial;

class Material
{
	friend class Gui;
public:
	Material(Graphics& graphics, const aiMaterial* const assignedMaterial, const ShaderSettings shaderSettings);
	void Bind(Graphics& graphics) noexcept;

private:
	std::vector<std::unique_ptr<Bindable>> bindables;
	std::vector<std::shared_ptr<Bindable>> sharedBindables;

	struct Roughness
	{
		alignas(16) float roughness = 0.8f;
	} roughnessBuffer;

	struct Color
	{
		DirectX::XMFLOAT4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
	};

	std::unique_ptr<Color> colorBuffer = nullptr;

	static const std::unordered_map<ShaderSettings, std::wstring, ShaderSettingsHash> psPaths;
};
