#include "DepthStencilView.h"
#include "Graphics.h"
#include "ThrowMacros.h"

DepthStencilView::DepthStencilView(Graphics& graphics, const Usage usage, const float clearValue, const UINT width, const UINT height) :
	usage(usage),
	clearValue(clearValue)
{
	auto resourceFlags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	DXGI_FORMAT format{};
	switch (usage)
	{
	case Usage::DepthStencil:
		format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		resourceFlags |= D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE;
		break;
	case Usage::Depth:
		format = DXGI_FORMAT_D32_FLOAT;
		resourceFlags |= D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE;
		break;
	case Usage::DepthShadowMapping:
		format = DXGI_FORMAT_D32_FLOAT;
		break;
	default:
		break;
	}
	const CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_DEFAULT);
	const CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		format,
		width, height,
		1, 0, 1, 0,
		resourceFlags);
	const D3D12_CLEAR_VALUE clearValueDesc = {
		.Format = format,
		.DepthStencil = { clearValue, 0 },
	};
	CHECK_HR(graphics.GetDevice()->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&clearValueDesc,
		IID_PPV_ARGS(&depthBuffer)));

	const D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {
		.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
		.NumDescriptors = 1,
	};
	CHECK_HR(graphics.GetDevice()->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&dsvHeap)));

	const D3D12_DEPTH_STENCIL_VIEW_DESC dsViewDesk =
	{
		.Format = DXGI_FORMAT_D32_FLOAT,
		.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D
	};

	// dsv and handle
	dsvHandle = { dsvHeap->GetCPUDescriptorHandleForHeapStart() };
	graphics.GetDevice()->CreateDepthStencilView(depthBuffer.Get(), &dsViewDesk, dsvHandle);
}

CD3DX12_CPU_DESCRIPTOR_HANDLE DepthStencilView::GetDsvHandle() const noexcept
{
	return dsvHandle;
}

void DepthStencilView::Clear(ID3D12GraphicsCommandList* const commandList)
{
	D3D12_CLEAR_FLAGS clearFlags = D3D12_CLEAR_FLAG_DEPTH;
	switch (usage)
	{
	case Usage::DepthStencil:
		clearFlags |= D3D12_CLEAR_FLAG_STENCIL;
		break;
	default:
		break;
	}
	commandList->ClearDepthStencilView(dsvHandle, clearFlags, clearValue, 0, 0, nullptr);
}

ID3D12Resource* DepthStencilView::GetBuffer() noexcept
{
	return depthBuffer.Get();
}
