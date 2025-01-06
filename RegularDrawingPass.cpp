#include "RegularDrawingPass.h"
#include "Graphics.h"
#include "Camera.h"
#include "Actor.h"
#include "ScissorRectangle.h"
#include "Viewport.h"
#include "RootParametersDescription.h"

RegularDrawingPass::RegularDrawingPass(Graphics& graphics, const Camera* camera, DirectX::XMFLOAT4X4 projection) :
	Pass(camera, projection)
{
	type = PassType::RegularDrawing;
	const float windowWidth = graphics.GetWindowWidth();
	const float windowHeight = graphics.GetWindowHeight();

	bindables.push_back(std::make_unique<ScissorRectangle>());
	bindables.push_back(std::make_unique<Viewport>(windowWidth, windowHeight));

	depthStencilView = std::make_unique<DepthStencilView>(graphics, DepthStencilView::Usage::Depth, 0.f, UINT(windowWidth), UINT(windowHeight));

	std::vector<CD3DX12_ROOT_PARAMETER> rootParameters;
	rootParameters.resize(RPD::paramsNum);

	for (const auto& cb : RPD::cbConsts)
	{
		rootParameters[cb.ParamIndex].InitAsConstants(cb.dataSize / 4, cb.slot, 0, cb.visibility);
	}

	for (const auto& cbv : RPD::cbvs)
	{
		rootParameters[cbv.ParamIndex].InitAsConstantBufferView(cbv.slot, 0u, cbv.visibility);
	}

	for (UINT i = 0; i < RPD::texturesNum; i++)
	{
		texesDescRanges.push_back(D3D12_DESCRIPTOR_RANGE{ D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1u, i, 0u, i });
	}
	rootParameters[RPD::ParamsIndexes::TexturesDescTable].InitAsDescriptorTable((UINT)texesDescRanges.size(), texesDescRanges.data(), D3D12_SHADER_VISIBILITY_PIXEL);
	rootSignature = std::make_unique<RootSignature>(graphics, rootParameters);

	pipelineStateStream.primitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	pipelineStateStream.renderTargetFormats =
	{
		.RTFormats{ Graphics::renderTargetDxgiFormat },
		.NumRenderTargets = 1,
	};
	pipelineStateStream.dsvFormat = DXGI_FORMAT_D32_FLOAT;
	auto dsDesc = CD3DX12_DEPTH_STENCIL_DESC(CD3DX12_DEFAULT{});
	dsDesc.DepthFunc = D3D12_COMPARISON_FUNC_GREATER;
	pipelineStateStream.depthStencil = dsDesc;
	pipelineStateStream.rootSignature = rootSignature->Get();
}

void RegularDrawingPass::Execute(Graphics& graphics, const std::vector<std::unique_ptr<Actor>>& actors)
{
	Pass::Execute(graphics, actors);

	graphics.ClearRenderTargetView();
	depthStencilView->Clear(graphics.GetMainCommandList());
	auto rtv = graphics.GetRtvCpuHandle();
	auto dsvHandle = depthStencilView->GetDsvHandle();
	graphics.GetMainCommandList()->OMSetRenderTargets(1, &rtv, TRUE, &dsvHandle);
	for (auto& actor : actors)
	{
		actor->Draw(graphics, PassType::RegularDrawing);
	}
}
