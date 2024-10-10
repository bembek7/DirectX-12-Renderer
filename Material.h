#pragma once
#include "BetterWindows.h"
#include "Bindable.h"
#include <vector>
#include <memory>
#include <DirectXMath.h>
#include <string>
#include <unordered_map>
#include "ShaderSettings.h"
#include "PipelineState.h"

struct aiMaterial;
class Texture;

class Material
{
	friend class Gui;
public:
	Material(Graphics& graphics, PipelineState::PipelineStateStream& pipelineStateStream, const aiMaterial* const assignedMaterial,
		ShaderSettings shaderSettings, std::vector<CD3DX12_ROOT_PARAMETER>& rootParameters);
	void Bind(Graphics& graphics) noexcept;

private:
	std::vector<std::unique_ptr<Bindable>> bindables;
	std::vector<std::shared_ptr<Texture>> textures;
	std::vector<D3D12_DESCRIPTOR_RANGE> texesDescRanges;
	UINT desciptorTableRootIndex;
	Microsoft::WRL::ComPtr<ID3DBlob> pixelShaderBlob;
	struct Roughness
	{
		alignas(16) float roughness = 0.8f;
	};

	std::unique_ptr<Roughness> roughnessBuffer = nullptr;

	struct Color
	{
		DirectX::XMFLOAT4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
	};

	std::unique_ptr<Color> colorBuffer = nullptr;

	static const std::unordered_map<ShaderSettings, std::wstring, ShaderSettingsHash> psPaths;
};
