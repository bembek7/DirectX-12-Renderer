#pragma once
#include "IndexBuffer.h"
#include "VertexBuffer.h"

class Graphcis;

class Mesh
{
public:
	Mesh(Graphics& graphics);
	void Bind(Graphics& graphics);
	void OnUpdate(const float time);

	UINT GetIndicesNumber() const noexcept;
private:
	std::unique_ptr<VertexBuffer> vertexBuffer;
	std::unique_ptr<IndexBuffer> indexBuffer;

	struct TransformBuffer
	{
		DirectX::XMFLOAT4X4 transformViewProj;
	} transformBuffer;

	DirectX::XMFLOAT4X4 transform;
	std::unique_ptr<Bindable> constantTransformBuffer;
};
