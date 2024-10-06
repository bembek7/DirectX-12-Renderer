#include "Mesh.h"
#include "Graphics.h"
#include "ConstantBuffer.h"
#include "RootSignature.h"
#include "PipelineState.h"
#include <d3dcompiler.h>

namespace Dx = DirectX;
namespace Wrl = Microsoft::WRL;

Mesh::Mesh(Graphics& graphics)
{
}

void Mesh::Bind(Graphics& graphics)
{
	Dx::XMStoreFloat4x4(&transformBuffer.transformViewProj, Dx::XMMatrixTranspose(Dx::XMLoadFloat4x4(&transform) * graphics.GetCamera() * graphics.GetProjection()));

	model->Bind(graphics);

	pipelineState->Bind(graphics);
	rootSignature->Bind(graphics);
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
	return model->GetIndicesNumber();
}