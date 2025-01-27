#pragma once
#include "Pass.h"
#include "ConstantBuffer.h"
#include "DepthStencilView.h"

class LightPerspectivePass : public Pass
{
public:
	LightPerspectivePass(Graphics& graphics, const Camera* camera, const DirectX::XMFLOAT4X4 projection);

	void Execute(Graphics& graphics, const std::vector<std::unique_ptr<Actor>>& actors);

	virtual void BindPassSpecific(ID3D12GraphicsCommandList* const drawingBundle) override;

	ID3D12Resource* GetDepthBuffer() noexcept;
private:
	std::unique_ptr<DepthStencilView> depthStencilView;
	DirectX::XMFLOAT4X4 projection{};
	const Camera* cameraUsed;
	std::unique_ptr<PipelineState> pipelineState;
};

