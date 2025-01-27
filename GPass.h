#pragma once
#include "Pass.h"
#include "DepthStencilView.h"
#include "PipelineState.h"
#include "RTVHeap.h"

class Graphics;
class Camera;
class Actor;

class GPass : public Pass
{
public:
	GPass(Graphics& graphics, const Camera* camera, DirectX::XMFLOAT4X4 projection);

	void Execute(Graphics& graphics, const std::vector<std::unique_ptr<Actor>>& actors);

	virtual void BindPassSpecific(ID3D12GraphicsCommandList* const drawingBundle) override;

	ID3D12Resource* GetColorTexture() noexcept;
	ID3D12Resource* GetNormal_RoughnessTexture() noexcept;
	ID3D12Resource* GetSpecularColorTexture() noexcept;
	ID3D12Resource* GetViewPositionTexture() noexcept;
	ID3D12Resource* GetDepthBuffer() noexcept;
private:
	std::unique_ptr<DepthStencilView> depthStencilView;
	std::unique_ptr<RTVHeap> rtvHeap;
	DirectX::XMFLOAT4X4 projection{};
	const Camera* cameraUsed;
	static constexpr UINT rtvsNum = 4;
};
