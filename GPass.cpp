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

	pipelineStateStream.renderTargetFormats =
	{
		.RTFormats{ DXGI_FORMAT_R11G11B10_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R11G11B10_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT },
		.NumRenderTargets = rtvsNum,
	};
	pipelineStateStream.dsvFormat = DXGI_FORMAT_D32_FLOAT;
	auto dsDesc = CD3DX12_DEPTH_STENCIL_DESC(CD3DX12_DEFAULT{});
	dsDesc.DepthFunc = D3D12_COMPARISON_FUNC_GREATER_EQUAL;
	pipelineStateStream.depthStencil = dsDesc;

	std::array<D3D12_RENDER_TARGET_VIEW_DESC, rtvsNum> rtvDescs =
	{
		D3D12_RENDER_TARGET_VIEW_DESC{ DXGI_FORMAT_R11G11B10_FLOAT, D3D12_RTV_DIMENSION_TEXTURE2D },
		D3D12_RENDER_TARGET_VIEW_DESC{ DXGI_FORMAT_R16G16B16A16_FLOAT, D3D12_RTV_DIMENSION_TEXTURE2D },
		D3D12_RENDER_TARGET_VIEW_DESC{ DXGI_FORMAT_R11G11B10_FLOAT, D3D12_RTV_DIMENSION_TEXTURE2D },
		D3D12_RENDER_TARGET_VIEW_DESC{ DXGI_FORMAT_R16G16B16A16_FLOAT, D3D12_RTV_DIMENSION_TEXTURE2D },
		D3D12_RENDER_TARGET_VIEW_DESC{ DXGI_FORMAT_R16G16B16A16_FLOAT, D3D12_RTV_DIMENSION_TEXTURE2D }
	}; // TODO do not repeat the formats

	rtvHeap = std::make_unique<RTVHeap>(graphics, rtvsNum, rtvDescs.data());
}

void GPass::Execute(Graphics& graphics, const std::vector<std::unique_ptr<Actor>>& actors)
{
	Pass::Execute(graphics);
	graphics.SetCamera(cameraUsed->GetMatrix());
	graphics.SetProjection(projection);

	depthStencilView->Clear(graphics.GetMainCommandList());
	auto rtv = rtvHeap->GetCPUHandle();
	
	for (UINT i = 0; i < rtvsNum; ++i)
	{
		auto rtvHandle = rtvHeap->GetCPUHandle(i);
		const float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		graphics.GetMainCommandList()->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	}
	
	auto dsvHandle = depthStencilView->GetDsvHandle();
	graphics.GetMainCommandList()->OMSetRenderTargets(rtvsNum, &rtv, TRUE, &dsvHandle);
	for (auto& actor : actors)
	{
		actor->Draw(graphics, GetType());
	}
}
void GPass::BindPassSpecific(ID3D12GraphicsCommandList* const drawingBundle)
{
	Pass::BindPassSpecific(drawingBundle);

	rootSignature->Bind(drawingBundle);
}


ID3D12Resource* GPass::GetColorTexture() noexcept
{
	return rtvHeap->GetRenderTarget(0);
}

ID3D12Resource* GPass::GetNormal_RoughnessTexture() noexcept
{
	return rtvHeap->GetRenderTarget(1);
}

ID3D12Resource* GPass::GetSpecularColorTexture() noexcept
{
	return rtvHeap->GetRenderTarget(2);
}

ID3D12Resource* GPass::GetViewPositionTexture() noexcept
{
	return rtvHeap->GetRenderTarget(3);
}

ID3D12Resource* GPass::GetWorldPositionTexture() noexcept
{
	return rtvHeap->GetRenderTarget(4);
}