#pragma once
#include "Bindable.h"
#include <vector>
#include <memory>
#include <DirectXMath.h>
#include "PipelineState.h"
#include "RootSignature.h"

class Graphics;

enum class PassType
{
	RegularDrawing,
	ShadowMapping,
	DepthPrePass,
};

class Pass
{
public:
	void Execute(Graphics& graphics);
	PipelineState::PipelineStateStream GetPSS() const { return pipelineStateStream; };
	virtual ~Pass() = default;
	PassType GetType() const noexcept;
protected:
	std::vector<std::unique_ptr<Bindable>> bindables;
	std::vector<std::shared_ptr<Bindable>> sharedBindables;
	DirectX::XMFLOAT4X4 projection{};
	PipelineState::PipelineStateStream pipelineStateStream;
	std::unique_ptr<RootSignature> rootSignature;
	PassType type;
};
