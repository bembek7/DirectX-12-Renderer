#pragma once
#include "Pass.h"
#include "ConstantBuffer.h"
#include "DepthStencilView.h"

class ShadowPass : public Pass
{
public:
	ShadowPass(Graphics& graphics, const Camera* camera, const DirectX::XMFLOAT4X4 projection, ID3D12Resource* const mainDepthBuffer);

	void Execute(Graphics& graphics, const std::vector<std::unique_ptr<Actor>>& actors, const CD3DX12_CPU_DESCRIPTOR_HANDLE& shadowMapRtvHandle);

	virtual void BindPassSpecific(ID3D12GraphicsCommandList* const drawingBundle) override;
	//virtual void BindPassSpecificRootParams(ID3D12GraphicsCommandList* const drawingBundle) override;
private:
	std::unique_ptr<DepthStencilView> depthStencilView;
	DirectX::XMFLOAT4X4 projection{};
	const Camera* cameraUsed;
	std::unique_ptr<PipelineState> pipelineState;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvHeap;
};

