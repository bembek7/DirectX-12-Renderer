#include "LightPerspectivePass.h"
#include "Viewport.h"
#include "Graphics.h"
#include "ScissorRectangle.h"
#include "Actor.h"
#include "Camera.h"
#include "DirectionalLight.h"
#include "RootParametersDescription.h"

namespace Dx = DirectX;

//LightPerspectivePass::LightPerspectivePass(Graphics& graphics, const Camera* camera, DirectX::XMFLOAT4X4 projection) :
//	Pass(camera, projection)
//{
//	type = PassType::LightPerspective;
//
//	const float windowWidth = graphics.GetWindowWidth();
//	const float windowHeight = graphics.GetWindowHeight();
//
//	bindables.push_back(std::make_unique<ScissorRectangle>());
//	bindables.push_back(std::make_unique<Viewport>(windowWidth, windowHeight));
//
//	depthStencilView = std::make_unique<DepthStencilView>(graphics, DepthStencilView::Usage::DepthShadowMapping, 1.f, UINT(windowWidth), UINT(windowHeight));
//
//	const D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
//		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
//		D3D12_ROOT_SIGNATURE_FLAG_DENY_MESH_SHADER_ROOT_ACCESS |
//		D3D12_ROOT_SIGNATURE_FLAG_DENY_AMPLIFICATION_SHADER_ROOT_ACCESS |
//		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
//		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
//		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
//		D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;
//	// define empty root signature
//	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
//
//	std::vector<CD3DX12_ROOT_PARAMETER> rootParameters;
//	rootParameters.resize(1);
//
//	for (const auto& cb : RPD::cbConsts)
//	{
//		rootParameters[cb.ParamIndex].InitAsConstants(cb.dataSize / 4, cb.slot, 0, cb.visibility); // binding transform buffer
//	}
//	rootSignatureDesc.Init(1, rootParameters.data(), 0, nullptr, rootSignatureFlags);
//
//	rootSignature = std::make_unique<RootSignature>(graphics, rootSignatureDesc);
//
//	pipelineStateStream.primitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
//	pipelineStateStream.renderTargetFormats =
//	{
//		.NumRenderTargets = 0,
//	};
//	pipelineStateStream.dsvFormat = DXGI_FORMAT_D32_FLOAT;
//	pipelineStateStream.depthStencil = CD3DX12_DEPTH_STENCIL_DESC(CD3DX12_DEFAULT{});
//	auto rasterizerDesc = CD3DX12_RASTERIZER_DESC(CD3DX12_DEFAULT{});
//	rasterizerDesc.CullMode = D3D12_CULL_MODE_FRONT;
//	pipelineStateStream.rasterizer = rasterizerDesc;
//	pipelineStateStream.rootSignature = rootSignature->Get();
//}

void LightPerspectivePass::Execute(Graphics& graphics, const std::vector<std::unique_ptr<Actor>>& actors)
{
	Pass::Execute(graphics, actors);

	namespace Dx = DirectX;
	Dx::XMStoreFloat4x4(&lightPerspective, Dx::XMMatrixTranspose(cameraUsed->GetMatrix() * Dx::XMLoadFloat4x4(&projection)));

	depthStencilView->Clear(graphics.GetMainCommandList());
	
	auto dsvHandle = depthStencilView->GetDsvHandle();
	graphics.GetMainCommandList()->OMSetRenderTargets(0, nullptr, TRUE, &dsvHandle);

	for (auto& actor : actors)
	{
		actor->Draw(graphics, GetType());
	}
}

ID3D12Resource* LightPerspectivePass::GetDepthBuffer()
{
	return depthStencilView->GetBuffer();
}

DirectX::XMFLOAT4X4 LightPerspectivePass::GetLightPerspective() const noexcept
{
	return lightPerspective;
}
