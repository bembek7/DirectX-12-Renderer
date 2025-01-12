#include "ShadowMapGenerationPass.h"
#include "ScissorRectangle.h"
#include "Viewport.h"
#include "RootParametersDescription.h"
#include "Actor.h"
#include "LightPerspectivePass.h"
#include "ThrowMacros.h"
#include "ShadersPool.h"

//ShadowMapGenerationPass::ShadowMapGenerationPass(Graphics& graphics, const Camera* camera, DirectX::XMFLOAT4X4 projection, LightPerspectivePass* const connectedLPpass):
//	Pass(camera, projection),
//	connectedLPpass(connectedLPpass)
//{
//	type = PassType::ShadowMapGeneraration;
//	providesShaders = true;
//
//	const float windowWidth = graphics.GetWindowWidth();
//	const float windowHeight = graphics.GetWindowHeight();
//
//	bindables.push_back(std::make_unique<ScissorRectangle>());
//	bindables.push_back(std::make_unique<Viewport>(windowWidth, windowHeight));
//
//	const D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
//		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
//		D3D12_ROOT_SIGNATURE_FLAG_DENY_MESH_SHADER_ROOT_ACCESS |
//		D3D12_ROOT_SIGNATURE_FLAG_DENY_AMPLIFICATION_SHADER_ROOT_ACCESS |
//		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
//		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
//		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
//	
//	// define empty root signature
//	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
//
//	std::vector<CD3DX12_ROOT_PARAMETER> rootParameters;
//	rootParameters.resize(3);
//
//	for (const auto& cb : RPD::cbConsts) // binding transform buffer, that's for rework
//	{
//		rootParameters[cb.ParamIndex].InitAsConstants(cb.dataSize / 4, cb.slot, 0u, cb.visibility);
//	} 
//
//	rootParameters[1].InitAsConstantBufferView(1u, 0u, D3D12_SHADER_VISIBILITY_VERTEX);
//
//	const D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc{
//			.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
//			.NumDescriptors = 1,
//			.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
//	};
//	CHECK_HR(graphics.GetDevice()->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&srvHeap)));
//
//	const D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {
//		.Format = DXGI_FORMAT_R32_FLOAT,
//		.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D,
//		.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
//		.Texture2D{.MipLevels = connectedLPpass->GetDepthBuffer()->GetDesc().MipLevels },
//	};
//	graphics.GetDevice()->CreateShaderResourceView(connectedLPpass->GetDepthBuffer(), &srvDesc, srvHeap->GetCPUDescriptorHandleForHeapStart());
//
//	CD3DX12_DESCRIPTOR_RANGE descriptorRange{ D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1U, 0U };
//	rootParameters[2].InitAsDescriptorTable(1u, &descriptorRange, D3D12_SHADER_VISIBILITY_PIXEL);
//
//	CD3DX12_STATIC_SAMPLER_DESC staticSamplers[1]{};
//
//	staticSamplers[0].Init(0, D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR);
//	staticSamplers[0].ShaderRegister = 0;
//	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
//	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
//	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
//	staticSamplers[0].BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
//	staticSamplers[0].Filter = D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
//	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
//	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
//
//	rootSignatureDesc.Init(3, rootParameters.data(), 1, staticSamplers, rootSignatureFlags);
//
//	rootSignature = std::make_unique<RootSignature>(graphics, rootSignatureDesc);
//
//	pipelineStateStream.primitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
//	pipelineStateStream.renderTargetFormats =
//	{
//		.RTFormats{ DXGI_FORMAT_R32_FLOAT },
//		.NumRenderTargets = 1,
//	};
//	pipelineStateStream.dsvFormat = DXGI_FORMAT_D32_FLOAT;
//	auto dsDesc = CD3DX12_DEPTH_STENCIL_DESC(CD3DX12_DEFAULT{});
//	dsDesc.DepthFunc = D3D12_COMPARISON_FUNC_GREATER_EQUAL;
//	dsDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
//	pipelineStateStream.depthStencil = dsDesc;
//	pipelineStateStream.rootSignature = rootSignature->Get();
//	std::wstring vertexShaderPath = L"ShadowMapGenerationVS.cso";
//	std::wstring pixelShaderPath = L"ShadowMapGenerationPS.cso";
//
//	auto& shadersPool = ShadersPool::GetInstance();
//	vsBlob = shadersPool.GetShaderBlob(vertexShaderPath);
//	psBlob = shadersPool.GetShaderBlob(pixelShaderPath);
//	pipelineStateStream.vertexShader = CD3DX12_SHADER_BYTECODE(vsBlob->Get());
//	pipelineStateStream.pixelShader = CD3DX12_SHADER_BYTECODE(psBlob->Get());
//
//	lightPerspectiveCB = std::make_unique<ConstantBufferCBV<LightPerspectiveBuffer>>(graphics, lightPerspectiveBuffer, 1u);
//}
//
//void ShadowMapGenerationPass::Execute(Graphics& graphics, const std::vector<std::unique_ptr<Actor>>& actors)
//{
//	Pass::Execute(graphics, actors);
//
//	graphics.ClearRenderTargetView();
//	auto rtv = graphics.GetRtvCpuHandle();
//	graphics.GetMainCommandList()->OMSetRenderTargets(1, &rtv, TRUE, graphics.GetDSVHandle());
//	for (auto& actor : actors)
//	{
//		actor->Draw(graphics, GetType());
//	}
//}

void ShadowMapGenerationPass::BindPassSpecificRootParams(ID3D12GraphicsCommandList* const drawingBundle)
{
	lightPerspectiveBuffer.lightPerspective = connectedLPpass->GetLightPerspective();
	lightPerspectiveCB->Update();
	lightPerspectiveCB->Bind(drawingBundle);

	drawingBundle->SetDescriptorHeaps(1u, srvHeap.GetAddressOf());
	drawingBundle->SetGraphicsRootDescriptorTable(2, srvHeap->GetGPUDescriptorHandleForHeapStart());
}
