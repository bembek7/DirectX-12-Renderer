#pragma once
#include "Bindable.h"
#include <vector>
#include <memory>
#include <DirectXMath.h>
#include "PipelineState.h"
#include "RootSignature.h"
#include "RootParametersDescription.h"

class Graphics;
class Camera;
class Actor;

enum class PassType
{
	GPass,
	ShadowMapGeneraration,
	LightPerspective,
	FinalPass
};

class Pass
{
public:
	Pass(PassType type, const std::vector<RPD::CBTypes>& constantBuffers = {}, const std::vector<RPD::TextureTypes>& textures = {}) noexcept;
	void Execute(Graphics& graphics);
	PipelineState::PipelineStateStream GetPSS() const { return pipelineStateStream; };
	virtual void BindPassSpecificRootParams(ID3D12GraphicsCommandList* const drawingBundle) {};
	virtual ID3D12DescriptorHeap* GetDescriptorHeap() noexcept { return srvHeap.Get(); };
	virtual ~Pass() = default;
	PassType GetType() const noexcept;
	RootSignature* GetRootSignature() noexcept;
	bool ProvidesShaders() const noexcept { return providesShaders; }
protected:
	std::vector<CD3DX12_ROOT_PARAMETER> InitRootParameters() noexcept;
protected:
	std::vector<std::unique_ptr<Bindable>> bindables;
	std::vector<std::shared_ptr<Bindable>> sharedBindables;
	PipelineState::PipelineStateStream pipelineStateStream;
	std::unique_ptr<RootSignature> rootSignature;
	PassType type{};
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvHeap;
	bool providesShaders = false;
	std::shared_ptr<Microsoft::WRL::ComPtr<ID3DBlob>> vsBlob;
	std::shared_ptr<Microsoft::WRL::ComPtr<ID3DBlob>> psBlob;
	std::vector<RPD::CBTypes> constantBuffers;
	std::vector<RPD::TextureTypes> textures;
	std::vector<D3D12_DESCRIPTOR_RANGE> texesDescRanges;
};
