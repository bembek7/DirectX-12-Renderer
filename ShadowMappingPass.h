#pragma once
#include "Pass.h"
#include "DepthStencilView.h"
#include "PipelineState.h"

class Actor;
class PointLight;
class DepthCubeTexture;
class Camera;
class Light;
class DirectionalLight;

class ShadowMappingPass : public Pass
{
public:
	ShadowMappingPass(Graphics& graphics, const Camera* camera, DirectX::XMFLOAT4X4 projection);

	virtual void Execute(Graphics& graphics, const std::vector<std::unique_ptr<Actor>>& actors) override;
private:
	std::unique_ptr<DepthStencilView> depthStencilView;
};
