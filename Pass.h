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
	ShadowMapping,
	DepthPrePass,
};

class Pass
{
public:
	Pass(const Camera* const camera, DirectX::XMFLOAT4X4 projection) noexcept;
	virtual void Execute(Graphics& graphics, const std::vector<std::unique_ptr<Actor>>& actors);
	PipelineState::PipelineStateStream GetPSS() const { return pipelineStateStream; };
	virtual ~Pass() = default;
	PassType GetType() const noexcept;
	RootSignature* GetRootSignature() noexcept;
protected:
	std::vector<std::unique_ptr<Bindable>> bindables;
	std::vector<std::shared_ptr<Bindable>> sharedBindables;
	PipelineState::PipelineStateStream pipelineStateStream;
	std::unique_ptr<RootSignature> rootSignature;
	PassType type{};
private:
	DirectX::XMFLOAT4X4 projection{};
	const Camera* cameraUsed;
};
