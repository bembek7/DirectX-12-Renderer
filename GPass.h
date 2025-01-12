#pragma once
#include "Pass.h"
#include "DepthStencilView.h"
#include "PipelineState.h"
#include "RTVHeap.h"

class Graphics;
class Camera;
class Actor;
class Light;
class DepthCubeTexture;

class GPass : public Pass
{
public:
	GPass(Graphics& graphics, const Camera* camera, DirectX::XMFLOAT4X4 projection);

	virtual void Execute(Graphics& graphics, const std::vector<std::unique_ptr<Actor>>& actors) override;
private:
	std::unique_ptr<DepthStencilView> depthStencilView;
	std::unique_ptr<RTVHeap> rtvHeap;
};
