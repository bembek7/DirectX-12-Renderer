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
#include "ConstantBuffer.h"
#include "Texture.h"

struct aiMaterial;

class Material
{
	friend class Gui;
public:
	Material(Graphics& graphics, const aiMaterial* const assignedMaterial, ShaderSettings& shaderSettings);
	void Bind(Graphics& graphics, ID3D12GraphicsCommandList* const commandList) noexcept;
	ID3D12DescriptorHeap* GetDescriptorHeap() noexcept;
	void Update();
	ID3DBlob* GetPSBlob() const noexcept;
	CD3DX12_RASTERIZER_DESC GetRasterizerDesc() const noexcept;
private:
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvHeap;
	std::vector<std::unique_ptr<ConstantBuffer>> cBuffers;
	std::vector<std::unique_ptr<Texture>> textures;
	std::shared_ptr<Microsoft::WRL::ComPtr<ID3DBlob>> pixelShaderBlob;
	CD3DX12_RASTERIZER_DESC rasterizerDesc;
	struct Roughness
	{
		alignas(16) float roughness = 0.8f;
	};

	std::unique_ptr<Roughness> roughnessBuffer = nullptr;

	struct Color
	{
		DirectX::XMFLOAT3 color = { 1.0f, 1.0f, 1.0f};
	};

	std::unique_ptr<Color> colorBuffer = nullptr;

	static const std::unordered_map<ShaderSettings, std::wstring, ShaderSettingsHash> psPaths;
	static const std::unordered_map<ShaderSettings, INT, ShaderSettingsHash> textureHighestSlotMap;
};
