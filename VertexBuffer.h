#pragma once
#include "Bindable.h"
#include <vector>
#include <DirectXMath.h>
#include <wrl/client.h>

class VertexBuffer : public Bindable
{
public:
	struct Vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 color;
	};

	//VertexBuffer(Graphics& graphics, const std::vector<float>& vertices, const unsigned int vertexSize);
	VertexBuffer(Graphics& graphics, const std::vector<Vertex>& vertices);
	virtual void Bind(Graphics& graphics) noexcept override;

	UINT GetVerticesNumber() const noexcept;

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;

	UINT verticesNum;
};