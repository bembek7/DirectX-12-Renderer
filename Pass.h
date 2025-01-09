#pragma once
#include "Bindable.h"
#include <vector>
#include <memory>
#include <DirectXMath.h>
#include "PipelineState.h"
#include "RootSignature.h"

class Graphics;
class Camera;
class Actor;

enum class PassType
{
	RegularDrawing,
	ShadowMapGeneraration,
	LightPerspective,
	DepthPrePass,
};

class Pass
{
public:
	Pass(const Camera* const camera, DirectX::XMFLOAT4X4 projection) noexcept;
	virtual void Execute(Graphics& graphics, const std::vector<std::unique_ptr<Actor>>& actors);
	PipelineState::PipelineStateStream GetPSS() const { return pipelineStateStream; };
	virtual void BindPassSpecificRootParams(ID3D12GraphicsCommandList* const drawingBundle) {};
	virtual ID3D12DescriptorHeap* GetDescriptorHeap() noexcept { return srvHeap.Get(); };
	virtual ~Pass() = default;
	PassType GetType() const noexcept;
	RootSignature* GetRootSignature() noexcept;
	bool ProvidesShaders() const noexcept { return providesShaders; }
protected:
	std::vector<std::unique_ptr<Bindable>> bindables;
	std::vector<std::shared_ptr<Bindable>> sharedBindables;
	PipelineState::PipelineStateStream pipelineStateStream;
	std::unique_ptr<RootSignature> rootSignature;
	PassType type{};
	DirectX::XMFLOAT4X4 projection{};
	const Camera* cameraUsed;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvHeap;
	bool providesShaders = false;
	std::shared_ptr<Microsoft::WRL::ComPtr<ID3DBlob>> vsBlob;
	std::shared_ptr<Microsoft::WRL::ComPtr<ID3DBlob>> psBlob;
};
