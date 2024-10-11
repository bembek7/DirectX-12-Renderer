#include "IndexBuffer.h"
#include "Bindable.h"
#include "Graphics.h"
#include "ThrowMacros.h"

namespace Dx = DirectX;
namespace Wrl = Microsoft::WRL;

IndexBuffer::IndexBuffer(Graphics& graphics, const std::vector<WORD>& indices)
{
	// set the index count
	indicesNum = (UINT)indices.size();

	indexBuffer = std::move(graphics.GenerateBufferFromData(indices));

	indexBufferView =
	{
		.BufferLocation = indexBuffer->GetGPUVirtualAddress(),
		.SizeInBytes = indicesNum * (UINT)sizeof(WORD),
		.Format = DXGI_FORMAT_R16_UINT,
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