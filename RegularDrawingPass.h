#pragma once
#include "Pass.h"
#include "DepthStencilView.h"
#include "PipelineState.h"

class Graphics;
class Camera;
class Actor;
class Light;
class DepthCubeTexture;

class RegularDrawingPass : public Pass
{
public:
	RegularDrawingPass(Graphics& graphics, const Camera* camera, DirectX::XMFLOAT4X4 projection);

	virtual void Execute(Graphics& graphics, const std::vector<std::unique_ptr<Actor>>& actors) override;
private:
	std::vector<D3D12_DESCRIPTOR_RANGE> texesDescRanges;
	std::unique_ptr<DepthStencilView> depthStencilView;
};
