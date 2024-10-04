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
	Dx::XMMATRIX viewProjection;
	{
		// setup view (camera) matrix
		const auto eyePosition = Dx::XMVectorSet(0, 0, -6, 1);
		const auto focusPoint = Dx::XMVectorSet(0, 0, 0, 1);
		const auto upDirection = Dx::XMVectorSet(0, 1, 0, 0);
		const auto view = Dx::XMMatrixLookAtLH(eyePosition, focusPoint, upDirection);
		// setup perspective projection matrix
		const auto aspectRatio = graphics.GetWindowWidth() / graphics.GetWindowHeight();
		const auto projection = Dx::XMMatrixPerspectiveFovLH(Dx::XMConvertToRadians(65.f), aspectRatio, 0.1f, 100.0f);
		// combine matrices
		viewProjection = XMMatrixMultiply(view, projection);
	}
	// bind the transformation matrix

	Dx::XMStoreFloat4x4(&transformBuffer.transformViewProj, Dx::XMMatrixTranspose(Dx::XMLoadFloat4x4(&transform) * viewProjection));

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