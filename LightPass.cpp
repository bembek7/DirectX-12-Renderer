#include "LightPass.h"
#include "ShadersPool.h"
#include "RTVHeap.h"

LightPass::LightPass(Graphics& graphics, ID3D12Resource* const sceneNormal_RoughnessTexture, ID3D12Resource* const sceneSpecularColor, ID3D12Resource* const sceneViewPosition, Light* const light) :
	Pass(graphics, PassType::FinalPass,
		{ RPD::CBTypes::LightProperties },
		{ RPD::TextureTypes::SceneNormal_Roughness, RPD::TextureTypes::SceneSpecularColor, RPD::TextureTypes::SceneViewPosition},
		{ RPD::SamplerTypes::Anisotropic }),
	light(light)
{
	std::array<D3D12_RENDER_TARGET_VIEW_DESC, 1> rtvDescs =
	{
		D3D12_RENDER_TARGET_VIEW_DESC{ DXGI_FORMAT_R11G11B10_FLOAT, D3D12_RTV_DIMENSION_TEXTURE2D }
	};

	rtvHeap = std::make_unique<RTVHeap>(graphics, 1, rtvDescs.data());

	pipelineStateStream.renderTargetFormats =
	{
		.RTFormats{ DXGI_FORMAT_R11G11B10_FLOAT },
		.NumRenderTargets = 1u,
	};
	std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout =
	{
		{ "POSITION", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 0u, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0u },
		{ "TEX_COORD", 0u, DXGI_FORMAT_R32G32_FLOAT, 0u, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0u }
	};
	pipelineStateStream.inputLayout = { inputLayout.data(), (UINT)inputLayout.size() };
	pipelineStateStream.rasterizer = CD3DX12_RASTERIZER_DESC(CD3DX12_DEFAULT{});
	auto& shadersPool = ShadersPool::GetInstance();
	auto vertexShaderBlob = shadersPool.GetShaderBlob(L"PixelOnlyVS.cso");
	auto pixelShaderBlob = shadersPool.GetShaderBlob(L"PointLightPassPS.cso");
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
			.NumDescriptors = 3,
			.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
	};
	CHECK_HR(graphics.GetDevice()->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&srvHeap)));

	CD3DX12_CPU_DESCRIPTOR_HANDLE srvCpuHandle{ srvHeap->GetCPUDescriptorHandleForHeapStart() };

	const D3D12_SHADER_RESOURCE_VIEW_DESC normal_roughnessSrvDesc = {
		.Format = DXGI_FORMAT_R16G16B16A16_FLOAT, // TODO change to getter
		.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D,
		.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
		.Texture2D{.MipLevels = sceneNormal_RoughnessTexture->GetDesc().MipLevels },
	};
	graphics.GetDevice()->CreateShaderResourceView(sceneNormal_RoughnessTexture, &normal_roughnessSrvDesc, srvCpuHandle);

	srvCpuHandle.Offset(1, graphics.GetCbvSrvDescriptorSize());

	const D3D12_SHADER_RESOURCE_VIEW_DESC specularColorSrvDesc = {
		.Format = DXGI_FORMAT_R11G11B10_FLOAT, // TODO change to getter
		.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D,
		.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
		.Texture2D{.MipLevels = sceneSpecularColor->GetDesc().MipLevels },
	};
	graphics.GetDevice()->CreateShaderResourceView(sceneSpecularColor, &specularColorSrvDesc, srvCpuHandle);

	srvCpuHandle.Offset(1, graphics.GetCbvSrvDescriptorSize());

	const D3D12_SHADER_RESOURCE_VIEW_DESC viewPositionSrvDesc = {
		.Format = DXGI_FORMAT_R11G11B10_FLOAT, // TODO change to getter
		.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D,
		.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
		.Texture2D{.MipLevels = sceneViewPosition->GetDesc().MipLevels },
	};
	graphics.GetDevice()->CreateShaderResourceView(sceneViewPosition, &viewPositionSrvDesc, srvCpuHandle);

	drawingBundle->SetDescriptorHeaps(1u, srvHeap.GetAddressOf());
	drawingBundle->SetGraphicsRootDescriptorTable(rootSignature->GetDescriptorTableIndex(), srvHeap->GetGPUDescriptorHandleForHeapStart());

	CHECK_HR(drawingBundle->Close());
}

void LightPass::Execute(Graphics& graphics)
{
	Pass::Execute(graphics);

	auto rtv = rtvHeap->GetCPUHandle();
	const float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	graphics.GetMainCommandList()->ClearRenderTargetView(rtvHeap->GetCPUHandle(), clearColor, 0, nullptr);
	
	graphics.GetMainCommandList()->OMSetRenderTargets(1, &rtv, TRUE, nullptr);

	graphics.GetMainCommandList()->SetDescriptorHeaps(1u, srvHeap.GetAddressOf());
	graphics.ExecuteBundle(drawingBundle.Get());
	light->Bind(graphics.GetMainCommandList());
	graphics.GetMainCommandList()->DrawIndexedInstanced(indexBuffer->GetIndicesNumber(), 1, 0, 0, 0);
}

ID3D12Resource* LightPass::GetLightMap() noexcept
{
	return nullptr;
}
