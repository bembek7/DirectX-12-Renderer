#include "Mesh.h"
#include "Graphics.h"
#include "ConstantBuffer.h"
#include "RootSignature.h"
#include "PipelineState.h"
#include <d3dcompiler.h>

namespace Dx = DirectX;
namespace Wrl = Microsoft::WRL;

Mesh::Mesh(Graphics& graphics)
{
	transform = {};
	const std::vector<VertexBuffer::Vertex> vertexData =
	{
			{ {-1.0f, -1.0f, -1.0f}, {0.0f, 0.0f, 0.0f} }, // 0
			{ {-1.0f,  1.0f, -1.0f}, {0.0f, 1.0f, 0.0f} }, // 1
			{ {1.0f,  1.0f, -1.0f}, {1.0f, 1.0f, 0.0f} }, // 2
			{ {1.0f, -1.0f, -1.0f}, {1.0f, 0.0f, 0.0f} }, // 3
			{ {-1.0f, -1.0f,  1.0f}, {0.0f, 0.0f, 1.0f} }, // 4
			{ {-1.0f,  1.0f,  1.0f}, {0.0f, 1.0f, 1.0f} }, // 5
			{ {1.0f,  1.0f,  1.0f}, {1.0f, 1.0f, 1.0f} }, // 6
			{ {1.0f, -1.0f,  1.0f}, {1.0f, 0.0f, 1.0f} }  // 7
	};
	const std::vector<WORD> indexData =
	{
		0, 1, 2, 0, 2, 3,
		4, 6, 5, 4, 7, 6,
		4, 5, 1, 4, 1, 0,
		3, 2, 6, 3, 6, 7,
		1, 5, 6, 1, 6, 2,
		4, 0, 3, 4, 3, 7
	};

	const std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	// Load the vertex shader.
	Wrl::ComPtr<ID3DBlob> vertexShaderBlob;
	CHECK_HR(D3DReadFileToBlob(L"VertexShader.cso", &vertexShaderBlob));

	// Load the pixel shader.
	Wrl::ComPtr<ID3DBlob> pixelShaderBlob;
	CHECK_HR(D3DReadFileToBlob(L"PixelShader.cso", &pixelShaderBlob));

	std::vector<CD3DX12_ROOT_PARAMETER> rootParameters;

	vertexBuffer = std::make_unique<VertexBuffer>(graphics, vertexData);
	indexBuffer = std::make_unique<IndexBuffer>(graphics, indexData);

	constantTransformBuffer = std::make_unique<ConstantBuffer<TransformBuffer>>(graphics, transformBuffer, BufferType::Vertex, 0u, rootParameters);

	rootSignature = std::make_unique<RootSignature>(graphics, rootParameters);

	// filling pso structure
	PipelineState::PipelineStateStream pipelineStateStream;
	pipelineStateStream.rootSignature = rootSignature->Get();
	pipelineStateStream.inputLayout = { inputLayout.data(), (UINT)std::size(inputLayout) };
	pipelineStateStream.primitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	pipelineStateStream.vertexShader = CD3DX12_SHADER_BYTECODE(vertexShaderBlob.Get());
	pipelineStateStream.pixelShader = CD3DX12_SHADER_BYTECODE(pixelShaderBlob.Get());
	pipelineStateStream.renderTargetFormats = {
		.RTFormats{ DXGI_FORMAT_R8G8B8A8_UNORM },
		.NumRenderTargets = 1,
	};

	pipelineState = std::make_unique<PipelineState>(graphics, pipelineStateStream);
}

void Mesh::Bind(Graphics& graphics)
{
	Dx::XMStoreFloat4x4(&transformBuffer.transformViewProj, Dx::XMMatrixTranspose(Dx::XMLoadFloat4x4(&transform) * graphics.GetCamera() * graphics.GetProjection()));

	vertexBuffer->Bind(graphics);
	indexBuffer->Bind(graphics);

	pipelineState->Bind(graphics);
	rootSignature->Bind(graphics);
	constantTransformBuffer->Bind(graphics);
}

void Mesh::OnUpdate(const float time)
{
	Dx::XMStoreFloat4x4(&transform,
		Dx::XMMatrixRotationX(1.0f * time + 1.f) *
		Dx::XMMatrixRotationY(1.2f * time + 2.f) *
		Dx::XMMatrixRotationZ(1.1f * time + 0.f));
}

UINT Mesh::GetIndicesNumber() const noexcept
{
	return indexBuffer->GetIndicesNumber();
}