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
	Material(Graphics& graphics, PipelineState::PipelineStateStream& pipelineStateStream, const aiMaterial* const assignedMaterial,
		ShaderSettings shaderSettings);
	void Bind(Graphics& graphics, ID3D12GraphicsCommandList* const commandList) noexcept;
	void Update();
private:
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvHeap;
	CD3DX12_CPU_DESCRIPTOR_HANDLE srvCpuStartHandle;
	std::vector<std::unique_ptr<Updatable>> cBuffers;
	std::vector<std::unique_ptr<Texture>> textures;
	//UINT desciptorTableRootIndex;
	std::shared_ptr<Microsoft::WRL::ComPtr<ID3DBlob>> pixelShaderBlob;
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
	static const std::unordered_map<ShaderSettings, INT, ShaderSettingsHash> textureHighestSlotMap;
};
