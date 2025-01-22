#include "GPass.h"
#include "Graphics.h"
#include "Camera.h"
#include "Actor.h"
#include "ScissorRectangle.h"
#include "Viewport.h"
#include "RootParametersDescription.h"
#include <array>



GPass::GPass(Graphics& graphics, const Camera* camera, DirectX::XMFLOAT4X4 projection) :
	Pass(graphics, PassType::GPass, 
		{ RPD::CBTypes::Transform, RPD::CBTypes::Roughness, RPD::CBTypes::Color },
		{ RPD::TextureTypes::Diffuse, RPD::TextureTypes::NormalMap, RPD::TextureTypes::SpecularMap },
		{ RPD::SamplerTypes::Anisotropic }),
	cameraUsed(camera),
	projection(projection)
{
	const float windowWidth = graphics.GetWindowWidth();
	const float windowHeight = graphics.GetWindowHeight();

	depthStencilView = std::make_unique<DepthStencilView>(graphics, DepthStencilView::Usage::Depth, 0.f, UINT(windowWidth), UINT(windowHeight));
	graphics.SetDSVHandle(depthStencilView->GetDsvHandle());

	constexpr auto rtvsNum = 3;

	pipelineStateStream.primitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	pipelineStateStream.renderTargetFormats =
	{
		.RTFormats{ DXGI_FORMAT_R11G11B10_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R11G11B10_FLOAT },
		.NumRenderTargets = rtvsNum,
	};
	pipelineStateStream.dsvFormat = DXGI_FORMAT_D32_FLOAT;
	auto dsDesc = CD3DX12_DEPTH_STENCIL_DESC(CD3DX12_DEFAULT{});
	dsDesc.DepthFunc = D3D12_COMPARISON_FUNC_GREATER_EQUAL;
	pipelineStateStream.depthStencil = dsDesc;
	pipelineStateStream.rootSignature = rootSignature->Get();

	std::array<D3D12_RENDER_TARGET_VIEW_DESC, rtvsNum> rtvDescs =
	{
		D3D12_RENDER_TARGET_VIEW_DESC{ DXGI_FORMAT_R11G11B10_FLOAT, D3D12_RTV_DIMENSION_TEXTURE2D },
		D3D12_RENDER_TARGET_VIEW_DESC{ DXGI_FORMAT_R16G16B16A16_FLOAT, D3D12_RTV_DIMENSION_TEXTURE2D },
		D3D12_RENDER_TARGET_VIEW_DESC{ DXGI_FORMAT_R11G11B10_FLOAT, D3D12_RTV_DIMENSION_TEXTURE2D }
	};

	rtvHeap = std::make_unique<RTVHeap>(graphics, 3, rtvDescs.data());
}

void GPass::Execute(Graphics& graphics, const std::vector<std::unique_ptr<Actor>>& actors)
{
	Pass::Execute(graphics);
	graphics.SetCamera(cameraUsed->GetMatrix());
	graphics.SetProjection(projection);

	graphics.ClearRenderTargetView(); // to move
	auto rtv = rtvHeap->GetCPUHandle();
	graphics.GetMainCommandList()->OMSetRenderTargets(3, &rtv, TRUE, graphics.GetDSVHandle());
	for (auto& actor : actors)
	{
		actor->Draw(graphics, GetType());
	}
}
