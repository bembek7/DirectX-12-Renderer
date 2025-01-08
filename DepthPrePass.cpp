#include "DepthPrePass.h"
#include "Graphics.h"
#include <imgui_internal.h>
#include "ScissorRectangle.h"
#include "Viewport.h"
#include "Actor.h"
#include "Camera.h"
#include "RootParametersDescription.h"

DepthPrePass::DepthPrePass(Graphics& graphics, const Camera* camera, DirectX::XMFLOAT4X4 projection) :
	Pass(camera, projection)
{
	type = PassType::DepthPrePass;
	const float windowWidth = graphics.GetWindowWidth();
	const float windowHeight = graphics.GetWindowHeight();

	depthStencilView = std::make_unique<DepthStencilView>(graphics, DepthStencilView::Usage::Depth, 0.f, UINT(windowWidth), UINT(windowHeight));

	const D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_MESH_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_AMPLIFICATION_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;
	// define empty root signature
	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc{};

	std::vector<CD3DX12_ROOT_PARAMETER> rootParameters;
	rootParameters.resize(1);

	for (const auto& cb : RPD::cbConsts)
	{
		rootParameters[cb.ParamIndex].InitAsConstants(cb.dataSize / 4, cb.slot, 0, cb.visibility); // binding transform buffer
	}
	rootSignatureDesc.Init(1, rootParameters.data(), 0, nullptr, rootSignatureFlags);

	rootSignature = std::make_unique<RootSignature>(graphics, rootSignatureDesc);

	pipelineStateStream.primitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	pipelineStateStream.renderTargetFormats =
	{
		.NumRenderTargets = 0,
	};
	pipelineStateStream.dsvFormat = DXGI_FORMAT_D32_FLOAT;
	auto dsDesc = CD3DX12_DEPTH_STENCIL_DESC(CD3DX12_DEFAULT{});
	dsDesc.DepthFunc = D3D12_COMPARISON_FUNC_GREATER;
	pipelineStateStream.depthStencil = dsDesc;
	pipelineStateStream.rootSignature = rootSignature->Get();
}

void DepthPrePass::Execute(Graphics& graphics, const std::vector<std::unique_ptr<Actor>>& actors)
{
	Pass::Execute(graphics, actors);

	depthStencilView->Clear(graphics.GetMainCommandList());
	auto dsvHandle = depthStencilView->GetDsvHandle();
	graphics.GetMainCommandList()->OMSetRenderTargets(0, nullptr, TRUE, &dsvHandle);

	for (auto& actor : actors)
	{
		actor->Draw(graphics, PassType::DepthPrePass);
	}
}