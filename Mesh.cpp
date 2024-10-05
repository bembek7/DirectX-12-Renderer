#include "Mesh.h"
#include "Graphics.h"
#include "ConstantBuffer.h"

namespace Dx = DirectX;

Mesh::Mesh(Graphics& graphics)
{
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
	vertexBuffer = std::make_unique<VertexBuffer>(graphics, vertexData);
	indexBuffer = std::make_unique<IndexBuffer>(graphics, indexData);

	constantTransformBuffer = std::make_unique<ConstantBuffer<TransformBuffer>>(graphics, transformBuffer, BufferType::Vertex, 0u);
}

void Mesh::Bind(Graphics& graphics)
{
	Dx::XMStoreFloat4x4(&transformBuffer.transformViewProj, Dx::XMMatrixTranspose(Dx::XMLoadFloat4x4(&transform) * graphics.GetCamera() * graphics.GetProjection()));

	vertexBuffer->Bind(graphics);
	indexBuffer->Bind(graphics);
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