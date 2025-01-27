#include "ShadowPass.h"
#include "RootParametersDescription.h"
#include "Actor.h"
#include "LightPerspectivePass.h"
#include "ThrowMacros.h"
#include "ShadersPool.h"
#include "Graphics.h"
#include "Camera.h"

//void ShadowMapGenerationPass::BindPassSpecificRootParams(ID3D12GraphicsCommandList* const drawingBundle)
//{
//	lightPerspectiveBuffer.lightPerspective = connectedLPpass->GetLightPerspective();
//	lightPerspectiveCB->Update();
//	lightPerspectiveCB->Bind(drawingBundle);
//
//	//drawingBundle->SetDescriptorHeaps(1u, srvHeap.GetAddressOf());
//	//drawingBundle->SetGraphicsRootDescriptorTable(2, srvHeap->GetGPUDescriptorHandleForHeapStart());
//}

ShadowPass::ShadowPass(Graphics& graphics, const Camera* camera, const DirectX::XMFLOAT4X4 projection, ID3D12Resource* const mainDepthBuffer) :
	Pass(graphics, PassType::ShadowPass,
		{ RPD::CBTypes::Transform },
		{ RPD::TextureTypes::DepthBuffer },
		{ RPD::SamplerTypes::Comparison }),
	cameraUsed(camera),
	projection(projection)
{
	const float windowWidth = graphics.GetWindowWidth();
	const float windowHeight = graphics.GetWindowHeight();

	depthStencilView = std::make_unique<DepthStencilView>(graphics, DepthStencilView::Usage::Depth, 0.f, UINT(windowWidth), UINT(windowHeight));

	pipelineStateStream.renderTargetFormats =
	{
		.RTFormats{ DXGI_FORMAT_R32_FLOAT },
		.NumRenderTargets = 1,
	};

	pipelineStateStream.dsvFormat = DXGI_FORMAT_D32_FLOAT;
	auto dsDesc = CD3DX12_DEPTH_STENCIL_DESC(CD3DX12_DEFAULT{});
	dsDesc.DepthFunc = D3D12_COMPARISON_FUNC_GREATER_EQUAL;
	pipelineStateStream.depthStencil = dsDesc;

	auto blendDesc = CD3DX12_BLEND_DESC(CD3DX12_DEFAULT{});
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	pipelineStateStream.blendDesc = blendDesc;

	std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout =
	{
		{ "POSITION", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 0u, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0u },
	};
	pipelineStateStream.inputLayout = { inputLayout.data(), (UINT)inputLayout.size() };

	auto& shadersPool = ShadersPool::GetInstance();
	auto vertexShaderBlob = shadersPool.GetShaderBlob(L"ShadowPassVS.cso");
	auto pixelShaderBlob = shadersPool.GetShaderBlob(L"ShadowPassPS.cso");
	pipelineStateStream.vertexShader = CD3DX12_SHADER_BYTECODE(vertexShaderBlob->Get());
	pipelineStateStream.pixelShader = CD3DX12_SHADER_BYTECODE(pixelShaderBlob->Get());

	pipelineState = std::make_unique<PipelineState>(graphics, pipelineStateStream);

	const D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc{
			.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
			.NumDescriptors = 1u,
			.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
	};
	CHECK_HR(graphics.GetDevice()->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&srvHeap)));

	CD3DX12_CPU_DESCRIPTOR_HANDLE srvCpuHandle{ srvHeap->GetCPUDescriptorHandleForHeapStart() };

	graphics.CreateSRV(mainDepthBuffer, srvCpuHandle);
}

void ShadowPass::Execute(Graphics& graphics, const std::vector<std::unique_ptr<Actor>>& actors, const CD3DX12_CPU_DESCRIPTOR_HANDLE& shadowMapRtvHandle)
{
	Pass::Execute(graphics);
	graphics.SetCamera(cameraUsed->GetMatrix());
	graphics.SetProjection(projection);
	
	auto dsv = depthStencilView->GetDsvHandle(); 
	graphics.GetMainCommandList()->OMSetRenderTargets(1, &shadowMapRtvHandle, TRUE, &dsv);
	graphics.GetMainCommandList()->SetDescriptorHeaps(1u, srvHeap.GetAddressOf());
	for (auto& actor : actors)
	{
		actor->Draw(graphics, GetType());
	}
}

void ShadowPass::BindPassSpecific(ID3D12GraphicsCommandList* const drawingBundle)
{
	Pass::BindPassSpecific(drawingBundle);

	pipelineState->Bind(drawingBundle);
	rootSignature->Bind(drawingBundle);

	//bind cb if there will be any

	drawingBundle->SetDescriptorHeaps(1u, srvHeap.GetAddressOf());
	// TODO get rid of magic number
	drawingBundle->SetGraphicsRootDescriptorTable(1u, srvHeap->GetGPUDescriptorHandleForHeapStart());
}

