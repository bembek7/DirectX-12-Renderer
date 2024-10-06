#pragma once
#include "Bindable.h"
#include <vector>
#include <DirectXMath.h>
#include <wrl/client.h>

class VertexBuffer : public Bindable
{
public:
	VertexBuffer(Graphics& graphics, const std::vector<float>& verticesData, const UINT vertexSize, const UINT verticesNum);
	virtual void Bind(Graphics& graphics) noexcept override;

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;

	UINT verticesNum;
};