#include "RegularDrawingPass.h"
#include "Graphics.h"
#include "Camera.h"
#include "Actor.h"
#include "ScissorRectangle.h"
#include "Viewport.h"

RegularDrawingPass::RegularDrawingPass(Graphics& graphics)
{
	const float windowWidth = graphics.GetWindowWidth();
	const float windowHeight = graphics.GetWindowHeight();

	bindables.push_back(std::make_unique<ScissorRectangle>());
	bindables.push_back(std::make_unique<Viewport>(windowWidth, windowHeight));

	depthStencilView = std::make_unique<DepthStencilView>(graphics, DepthStencilView::Usage::Depth, 0.f, UINT(windowWidth), UINT(windowHeight));

	DirectX::XMMATRIX reverseZ =
	{
		1.0f, 0.0f,  0.0f, 0.0f,
		0.0f, 1.0f,  0.0f, 0.0f,
		0.0f, 0.0f, -1.0f, 0.0f,
		0.0f, 0.0f,  1.0f, 1.0f
	};
	DirectX::XMStoreFloat4x4(&projection, DirectX::XMMatrixPerspectiveLH(1.0f, windowHeight / windowWidth, 0.5f, 200.0f) * reverseZ);
}

void RegularDrawingPass::Execute(Graphics& graphics, const std::vector<std::unique_ptr<Actor>>& actors, const std::vector<Light*>& lights, const Camera* const mainCamera)
{
	Pass::Execute(graphics);

	for (auto& actor : actors)
	{
		actor->Update(graphics);
	}

	graphics.ClearRenderTargetView();
	depthStencilView->Clear(graphics.GetMainCommandList());
	auto rtv = graphics.GetRtvCpuHandle();
	auto dsvHandle = depthStencilView->GetDsvHandle();
	graphics.GetMainCommandList()->OMSetRenderTargets(1, &rtv, TRUE, &dsvHandle);

	graphics.SetCamera(mainCamera->GetMatrix());
	for (auto& actor : actors)
	{
		actor->Draw(graphics, lights);
	}
}

PipelineState::PipelineStateStream RegularDrawingPass::GetCommonPSS() noexcept
{
	PipelineState::PipelineStateStream commonPipelineStateStream;
	commonPipelineStateStream.primitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	commonPipelineStateStream.renderTargetFormats =
	{
		.RTFormats{ Graphics::renderTargetDxgiFormat },
		.NumRenderTargets = 1,
	};
	commonPipelineStateStream.dsvFormat = DXGI_FORMAT_D32_FLOAT;
	auto dsDesc = CD3DX12_DEPTH_STENCIL_DESC(CD3DX12_DEFAULT{});
	dsDesc.DepthFunc = D3D12_COMPARISON_FUNC_GREATER;
	commonPipelineStateStream.depthStencil = dsDesc;

	return commonPipelineStateStream;
}