#pragma once
#include "Pass.h"
#include "ConstantBuffer.h"
#include "DepthStencilView.h"
#include "Light.h"

class LightPerspectivePass : public Pass
{
public:
	LightPerspectivePass(Graphics& graphics, Camera* camera, const DirectX::XMFLOAT4X4 projection, const LightType lightType);

	void Execute(Graphics& graphics, const std::vector<std::unique_ptr<Actor>>& actors);
	void RenderFace(Graphics& graphics, const std::vector<std::unique_ptr<Actor>>& actors, const DirectX::XMFLOAT3& cameraRotation, const D3D12_CPU_DESCRIPTOR_HANDLE* dsvHandle);

	virtual void BindPassSpecific(ID3D12GraphicsCommandList* const drawingBundle) override;

	ID3D12Resource* GetDepthBuffer() noexcept;
private:
	std::unique_ptr<DepthStencilView> depthStencilView;
	DirectX::XMFLOAT4X4 projection{};
	Camera* cameraUsed;
	std::unique_ptr<PipelineState> pipelineState;
	bool usesDepthCube = false;
};

