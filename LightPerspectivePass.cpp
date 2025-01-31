#include "LightPerspectivePass.h"
#include "RootParametersDescription.h"
#include "Actor.h"
#include "ThrowMacros.h"
#include "ShadersPool.h"
#include "Graphics.h"
#include "Camera.h"
#include "Viewport.h"

LightPerspectivePass::LightPerspectivePass(Graphics& graphics, Camera* camera, const DirectX::XMFLOAT4X4 projection, const LightType lightType) :
	Pass(graphics, PassType::LightPerspectivePass,
		{ RPD::CBTypes::Transform }),
	cameraUsed(camera),
	projection(projection)
{
	float dsvWidth = graphics.GetWindowWidth();
	float dsvHeight = graphics.GetWindowHeight();

	auto dsvUsage = DepthStencilView::Usage::Depth;
	if (lightType == LightType::Point)
	{
		usesDepthCube = true;
		dsvUsage = DepthStencilView::Usage::DepthCube;
		dsvWidth = 1024.f;
		dsvHeight = 1024.f;
	}
	bindables.push_back(std::make_unique<Viewport>(dsvWidth, dsvHeight));
	depthStencilView = std::make_unique<DepthStencilView>(graphics, dsvUsage, 0.f, UINT(dsvWidth), UINT(dsvHeight));

	pipelineStateStream.renderTargetFormats =
	{
		.RTFormats{ },
		.NumRenderTargets = 0u,
	};

	pipelineStateStream.dsvFormat = DXGI_FORMAT_D32_FLOAT;
	auto dsDesc = CD3DX12_DEPTH_STENCIL_DESC(CD3DX12_DEFAULT{});
	dsDesc.DepthFunc = D3D12_COMPARISON_FUNC_GREATER_EQUAL;
	pipelineStateStream.depthStencil = dsDesc;
	auto rasterizer = CD3DX12_RASTERIZER_DESC(CD3DX12_DEFAULT{});
	rasterizer.CullMode = D3D12_CULL_MODE_FRONT;
	pipelineStateStream.rasterizer = rasterizer;

	std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout =
	{
		{ "POSITION", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 0u, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0u },
	};
	pipelineStateStream.inputLayout = { inputLayout.data(), (UINT)inputLayout.size() };

	auto& shadersPool = ShadersPool::GetInstance();
	auto vertexShaderBlob = shadersPool.GetShaderBlob(L"LightPerspectivePassVS.cso");
	pipelineStateStream.vertexShader = CD3DX12_SHADER_BYTECODE(vertexShaderBlob->Get());

	pipelineState = std::make_unique<PipelineState>(graphics, pipelineStateStream);
}

void LightPerspectivePass::Execute(Graphics& graphics, const std::vector<std::unique_ptr<Actor>>& actors)
{
	Pass::Execute(graphics);
	graphics.SetProjection(projection);
	
	namespace Dx = DirectX;
	depthStencilView->Clear(graphics.GetMainCommandList());
	auto dsv = depthStencilView->GetDsvHandle();
	//front
	RenderFace(graphics, actors, Dx::XMFLOAT3{ 0.f, 0.f, 0.f }, &dsv);
	dsv.Offset(graphics.GetDsvDescriptorSize());

	if (usesDepthCube)
	{
		constexpr std::array<Dx::XMFLOAT3, 5> otherRotations =
		{
			Dx::XMFLOAT3{ 0.f, 180.f, 0.f },
			Dx::XMFLOAT3{ 0.f, 90.f, 0.f },
			Dx::XMFLOAT3{ 0.f, 270.f, 0.f },
			Dx::XMFLOAT3{ 90.f, 0.f, 0.f },
			Dx::XMFLOAT3{ 270.f, 0.f, 0.f }
		};
		for (const auto& rot : otherRotations)
		{
			RenderFace(graphics, actors, rot, &dsv);
			dsv.Offset(graphics.GetDsvDescriptorSize());
		}
	}
}

void LightPerspectivePass::RenderFace(Graphics& graphics, const std::vector<std::unique_ptr<Actor>>& actors, const DirectX::XMFLOAT3& cameraRotation, const D3D12_CPU_DESCRIPTOR_HANDLE* dsvHandle)
{
	cameraUsed->SetRelativeRotation(cameraRotation);
	graphics.SetCamera(cameraUsed->GetMatrix());
	graphics.GetMainCommandList()->OMSetRenderTargets(0, nullptr, TRUE, dsvHandle);
	for (auto& actor : actors)
	{
		actor->Draw(graphics, GetType());
	}
}

void LightPerspectivePass::BindPassSpecific(ID3D12GraphicsCommandList* const drawingBundle)
{
	Pass::BindPassSpecific(drawingBundle);

	pipelineState->Bind(drawingBundle);
	rootSignature->Bind(drawingBundle);
}

ID3D12Resource* LightPerspectivePass::GetDepthBuffer() noexcept
{
	return depthStencilView->GetBuffer();
}
