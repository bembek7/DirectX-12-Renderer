#pragma once
#include <vector>
#include "Bindable.h"
#include <wrl\client.h>

class Graphics;

class IndexBuffer : public Bindable
{
public:
	IndexBuffer(Graphics& graphics, const std::vector<WORD>& indices);
	virtual void Bind(ID3D12GraphicsCommandList* const commandList) noexcept override;
	UINT GetIndicesNumber() const noexcept;

private:
	D3D12_INDEX_BUFFER_VIEW indexBufferView;
	UINT indicesNum = 0;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexBuffer;
};
