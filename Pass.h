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
	LightPass,
	ShadowPass,
	LightPerspectivePass,
	FinalPass,
	RayTracingPass
};

class Pass
{
public:
	Pass(Graphics& graphics, PassType type, const std::vector<RPD::CBTypes>& constantBuffers = {}, const std::vector<RPD::TextureTypes>& textures = {}, const std::vector<RPD::SamplerTypes>& samplers = {});
	void Execute(Graphics& graphics);
	PipelineState::PipelineStateStream GetPSS() const { return pipelineStateStream; };
	virtual ~Pass() = default;
	PassType GetType() const noexcept;
	virtual void BindPassSpecific(ID3D12GraphicsCommandList* const drawingBundle);
protected:
	std::vector<std::unique_ptr<Bindable>> bindables;
	std::vector<std::shared_ptr<Bindable>> sharedBindables;
	PipelineState::PipelineStateStream pipelineStateStream;
	std::unique_ptr<RootSignature> rootSignature;
	PassType type{};
};
