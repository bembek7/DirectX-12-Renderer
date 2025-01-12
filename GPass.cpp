#include "GPass.h"
#include "Graphics.h"
#include "Camera.h"
#include "Actor.h"
#include "ScissorRectangle.h"
#include "Viewport.h"
#include "RootParametersDescription.h"
#include <array>



GPass::GPass(Graphics& graphics, const Camera* camera, DirectX::XMFLOAT4X4 projection) :
	Pass(camera, projection, PassType::GPass, 
		{ RPD::CBTypes::Transform, RPD::CBTypes::Roughness, RPD::CBTypes::Color },
		{ RPD::TextureTypes::Diffuse, RPD::TextureTypes::NormalMap, RPD::TextureTypes::SpecularMap })
{
	const float windowWidth = graphics.GetWindowWidth();
	const float windowHeight = graphics.GetWindowHeight();

	bindables.push_back(std::make_unique<ScissorRectangle>());
	bindables.push_back(std::make_unique<Viewport>(windowWidth, windowHeight));

	depthStencilView = std::make_unique<DepthStencilView>(graphics, DepthStencilView::Usage::Depth, 0.f, UINT(windowWidth), UINT(windowHeight));
	graphics.SetDSVHandle(depthStencilView->GetDsvHandle());

	const D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_MESH_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_AMPLIFICATION_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc{};

	CD3DX12_STATIC_SAMPLER_DESC staticSamplers[1]{};

	// Anisotropic sampler
	staticSamplers[0].Init(1u, D3D12_FILTER_ANISOTROPIC);
	staticSamplers[0].ShaderRegister = 1u;
	staticSamplers[0].Filter = D3D12_FILTER_ANISOTROPIC;
	staticSamplers[0].MaxAnisotropy = D3D12_REQ_MAXANISOTROPY;
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	
	const auto& rootParameters = InitRootParameters();
	rootSignatureDesc.Init((UINT)rootParameters.size(), rootParameters.data(), 1, staticSamplers, rootSignatureFlags);

	rootSignature = std::make_unique<RootSignature>(graphics, rootSignatureDesc);

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
	Pass::Execute(graphics, actors);

	graphics.ClearRenderTargetView();
	auto rtv = rtvHeap->GetCPUHandle();
	graphics.GetMainCommandList()->OMSetRenderTargets(3, &rtv, TRUE, graphics.GetDSVHandle());
	for (auto& actor : actors)
	{
		actor->Draw(graphics, GetType());
	}
}
