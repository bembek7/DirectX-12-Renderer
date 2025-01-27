#include "LightPass.h"
#include "ShadersPool.h"

using namespace RPD;

std::unordered_map<LightType, std::wstring> LightPass::shaderPaths =
{
	{ LightType::Point, L"PointLightPassPS.cso" },
	{ LightType::Spot, L"SpotLightPassPS.cso" },
	{ LightType::Directional, L"DirectionalLightPassPS.cso" }
};

LightPass::LightPass(Graphics& graphics, ID3D12Resource* const sceneNormal_RoughnessTexture, ID3D12Resource* const sceneSpecularColor,
	ID3D12Resource* const sceneViewPosition, ID3D12Resource* const sceneWorldPosition, ID3D12Resource* const lightDepthBuffer,
	Light* const light) :
	Pass(graphics, PassType::LightPass,
		{ CBTypes::LightProperties, CBTypes::LightPerspective },
		{ TextureTypes::SceneNormal_Roughness, TextureTypes::SceneSpecularColor, TextureTypes::SceneViewPosition, TextureTypes::SceneWorldPosition, TextureTypes::LightDepthBuffer },
		{ SamplerTypes::Anisotropic, SamplerTypes::Comparison }),
	light(light)
{
	// TODO change render targets states
	pipelineStateStream.renderTargetFormats =
	{
		.RTFormats{ DXGI_FORMAT_R11G11B10_FLOAT },
		.NumRenderTargets = 1u,
	};
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
		{ "TEX_COORD", 0u, DXGI_FORMAT_R32G32_FLOAT, 0u, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0u }
	};
	pipelineStateStream.inputLayout = { inputLayout.data(), (UINT)inputLayout.size() };
	pipelineStateStream.rasterizer = CD3DX12_RASTERIZER_DESC(CD3DX12_DEFAULT{});
	auto& shadersPool = ShadersPool::GetInstance();
	auto vertexShaderBlob = shadersPool.GetShaderBlob(L"PixelOnlyVS.cso");
	auto pixelShaderBlob = shadersPool.GetShaderBlob(shaderPaths[light->GetType()]);
	pipelineStateStream.vertexShader = CD3DX12_SHADER_BYTECODE(vertexShaderBlob->Get());
	pipelineStateStream.pixelShader = CD3DX12_SHADER_BYTECODE(pixelShaderBlob->Get());
	pipelineStateStream.depthStencil = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEPTH_STENCIL_DESC{ .DepthEnable = FALSE, .StencilEnable = FALSE });

	pipelineState = std::make_unique<PipelineState>(graphics, pipelineStateStream);

	vertexBuffer = std::make_unique<VertexBuffer>(graphics, std::vector<float>
	{
		-1.f, 1.f, 0.f, 0.f, 0.f,
		3.f, 1.f, 0.f, 2.f, 0.f,
		-1.f, -3.f, 0.f, 0.f, 2.f,
	}, UINT(5 * sizeof(float)), 3u);

	indexBuffer = std::make_unique<IndexBuffer>(graphics, std::vector<WORD>{ 0, 1, 2 });

	drawingBundle = graphics.CreateBundle();
	drawingBundle->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pipelineState->Bind(drawingBundle.Get());
	rootSignature->Bind(drawingBundle.Get());

	vertexBuffer->Bind(drawingBundle.Get());
	indexBuffer->Bind(drawingBundle.Get());

	const D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc{
			.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
			.NumDescriptors = 5,
			.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
	};
	CHECK_HR(graphics.GetDevice()->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&srvHeap)));

	CD3DX12_CPU_DESCRIPTOR_HANDLE srvCpuHandle{ srvHeap->GetCPUDescriptorHandleForHeapStart() };

	graphics.CreateSRV(sceneNormal_RoughnessTexture, srvCpuHandle);

	srvCpuHandle.Offset(1, graphics.GetCbvSrvDescriptorSize());

	graphics.CreateSRV(sceneSpecularColor, srvCpuHandle);

	srvCpuHandle.Offset(1, graphics.GetCbvSrvDescriptorSize());

	graphics.CreateSRV(sceneViewPosition, srvCpuHandle);

	srvCpuHandle.Offset(1, graphics.GetCbvSrvDescriptorSize());

	graphics.CreateSRV(sceneWorldPosition, srvCpuHandle);

	srvCpuHandle.Offset(1, graphics.GetCbvSrvDescriptorSize());

	graphics.CreateSRV(lightDepthBuffer, srvCpuHandle);

	drawingBundle->SetDescriptorHeaps(1u, srvHeap.GetAddressOf());
	drawingBundle->SetGraphicsRootDescriptorTable(rootSignature->GetDescriptorTableIndex(), srvHeap->GetGPUDescriptorHandleForHeapStart());

	CHECK_HR(drawingBundle->Close());
}

void LightPass::Execute(Graphics& graphics, const CD3DX12_CPU_DESCRIPTOR_HANDLE& lightMapRtvHandle)
{
	Pass::Execute(graphics);
	
	graphics.GetMainCommandList()->OMSetRenderTargets(1, &lightMapRtvHandle, TRUE, nullptr);

	graphics.GetMainCommandList()->SetDescriptorHeaps(1u, srvHeap.GetAddressOf());
	graphics.ExecuteBundle(drawingBundle.Get());
	light->Bind(graphics.GetMainCommandList());
	graphics.GetMainCommandList()->DrawIndexedInstanced(indexBuffer->GetIndicesNumber(), 1, 0, 0, 0);
}

ID3D12Resource* LightPass::GetLightMap() noexcept
{
	return nullptr;
}
