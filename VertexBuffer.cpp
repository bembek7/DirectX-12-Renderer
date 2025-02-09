#include "VertexBuffer.h"
#include "ThrowMacros.h"
#include <DirectXMath.h>
#include "d3dx12\d3dx12.h"
#include "Graphics.h"
#include "BufferLoader.h"

namespace Dx = DirectX;
namespace Wrl = Microsoft::WRL;

VertexBuffer::VertexBuffer(Graphics& graphics, const std::vector<float>& verticesData, const UINT vertexSize, const UINT verticesNum)
{
	vertexBuffer = std::move(BufferLoader::GenerateBufferFromData(graphics, verticesData, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));

	vertexBufferView =
	{
		.BufferLocation = vertexBuffer->GetGPUVirtualAddress(),
		.SizeInBytes = verticesNum * vertexSize,
		.StrideInBytes = vertexSize
	};
}

void VertexBuffer::Bind(ID3D12GraphicsCommandList* const commandList) noexcept
{
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
}

ID3D12Resource* VertexBuffer::GetBuffer()
{
	return vertexBuffer.Get();
}
