#pragma once
#include "Bindable.h"
#include <vector>
#include <DirectXMath.h>
#include <wrl/client.h>

class Graphics;

class VertexBuffer : public Bindable
{
public:
	VertexBuffer(Graphics& graphics, const std::vector<float>& verticesData, const UINT vertexSize, const UINT verticesNum);
	virtual void Bind(ID3D12GraphicsCommandList* const commandList) noexcept override;
	ID3D12Resource* GetBuffer();
private:
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
};