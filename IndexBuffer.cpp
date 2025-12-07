#include "IndexBuffer.h"
#include "Bindable.h"
#include "Graphics.h"
#include "ThrowMacros.h"
#include "BufferLoader.h"

namespace Dx = DirectX;
namespace Wrl = Microsoft::WRL;

IndexBuffer::IndexBuffer(Graphics& graphics, const std::vector<uint32_t>& indices)
{
	// set the index count
	indicesNum = (UINT)indices.size();

	indexBuffer = std::move(BufferLoader::GenerateBufferFromData(graphics, indices, D3D12_RESOURCE_STATE_INDEX_BUFFER));

	indexBufferView =
	{
		.BufferLocation = indexBuffer->GetGPUVirtualAddress(),
		.SizeInBytes = indicesNum * (UINT)sizeof(uint32_t),
		.Format = DXGI_FORMAT_R32_UINT,
	};
}

void IndexBuffer::Bind(ID3D12GraphicsCommandList* const commandList) noexcept
{
	commandList->IASetIndexBuffer(&indexBufferView);
}

UINT IndexBuffer::GetIndicesNumber() const noexcept
{
	return indicesNum;
}