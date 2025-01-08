#pragma once
#include "Pass.h"
#include "ConstantBuffer.h"

class LightPerspectivePass;

class ShadowMapGenerationPass : public Pass
{
public:
	ShadowMapGenerationPass(Graphics& graphics, const Camera* camera, DirectX::XMFLOAT4X4 projection, LightPerspectivePass* const connectedLPpass);

	virtual void Execute(Graphics& graphics, const std::vector<std::unique_ptr<Actor>>& actors) override;

	virtual void BindPassSpecificRootParams(ID3D12GraphicsCommandList* const drawingBundle) override;
private:
	struct LightPerspectiveBuffer
	{
		DirectX::XMFLOAT4X4 lightPerspective;
	}lightPerspectiveBuffer;

	std::unique_ptr<ConstantBuffer> lightPerspectiveCB;

	LightPerspectivePass* connectedLPpass;
};

