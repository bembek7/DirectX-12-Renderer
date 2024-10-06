#include "VertexBuffer.h"
#include "ThrowMacros.h"
#include <DirectXMath.h>
#include "d3dx12\d3dx12.h"
#include "Graphics.h"

namespace Dx = DirectX;
namespace Wrl = Microsoft::WRL;

VertexBuffer::VertexBuffer(Graphics& graphics, const std::vector<float>& verticesData, const UINT vertexSize, const UINT verticesNum)
{
	vertexBuffer = std::move(graphics.GenerateBufferFromData(verticesData));

	graphics.ResetCommandListAndAllocator();

	// transition vertex buffer to vertex buffer state
	{
		const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			vertexBuffer.Get(),
			D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
		GetCommandList(graphics)->ResourceBarrier(1, &barrier);
	}

	// close command list
	CHECK_HR(GetCommandList(graphics)->Close());

	graphics.ExecuteCommandList();

	WaitForQueueFinish(graphics);

	vertexBufferView =
	{
		.BufferLocation = vertexBuffer->GetGPUVirtualAddress(),
		.SizeInBytes = verticesNum * vertexSize,
		.StrideInBytes = vertexSize
	};
}

void VertexBuffer::Bind(Graphics& graphics) noexcept
{
	GetCommandList(graphics)->IASetVertexBuffers(0, 1, &vertexBufferView);
}