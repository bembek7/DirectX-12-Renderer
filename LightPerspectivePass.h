#pragma once
#include "Pass.h"
#include "DepthStencilView.h"
#include "PipelineState.h"
#include "ConstantBuffer.h"

class Actor;
class PointLight;
class DepthCubeTexture;
class Camera;
class Light;
class DirectionalLight;
class ConstantBuffer;

class LightPerspectivePass : public Pass
{
public:
	//LightPerspectivePass(Graphics& graphics, const Camera* camera, DirectX::XMFLOAT4X4 projection);

	//virtual void Execute(Graphics& graphics, const std::vector<std::unique_ptr<Actor>>& actors) override;

	ID3D12Resource* GetDepthBuffer();
	DirectX::XMFLOAT4X4 GetLightPerspective() const noexcept;
private:
	std::unique_ptr<DepthStencilView> depthStencilView;

	DirectX::XMFLOAT4X4 lightPerspective{};
};
