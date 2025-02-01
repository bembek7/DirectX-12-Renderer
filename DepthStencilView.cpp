#include "DepthStencilView.h"
#include "Graphics.h"
#include "ThrowMacros.h"

DepthStencilView::DepthStencilView(Graphics& graphics, const Usage usage, const float clearValue, const UINT width, const UINT height) :
	usage(usage),
	clearValue(clearValue)
{
	descSize = graphics.GetDsvDescriptorSize();
	auto resourceFlags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	DXGI_FORMAT format{};
	texturesNum = 1;
	switch (usage)
	{
	case Usage::DepthStencil:
		format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		resourceFlags |= D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE;
		break;
	case Usage::Depth:
		format = DXGI_FORMAT_D32_FLOAT;
		break;
	case Usage::DepthCube:
		format = DXGI_FORMAT_D32_FLOAT;
		texturesNum = 6;
		break;
	default:
		break;
	}
	const CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_DEFAULT);
	
	const CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		format,
		width, height,
		texturesNum, 0, 1, 0,
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
		.NumDescriptors = texturesNum,
	};
	CHECK_HR(graphics.GetDevice()->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&dsvHeap)));

	CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE{ dsvHeap->GetCPUDescriptorHandleForHeapStart() };
	for (UINT i = 0; i < texturesNum; i++)
	{
		D3D12_DEPTH_STENCIL_VIEW_DESC dsViewDesc = 
		{
			.Format = format,
			.ViewDimension = (usage == Usage::DepthCube) ? D3D12_DSV_DIMENSION_TEXTURE2DARRAY : D3D12_DSV_DIMENSION_TEXTURE2D,
			.Flags = D3D12_DSV_FLAG_NONE,
		};
		if (usage == Usage::DepthCube) 
		{
			dsViewDesc.Texture2DArray = 
			{ 
				.MipSlice = 0, 
				.FirstArraySlice = i,
				.ArraySize = 1 
			};
		}
		graphics.GetDevice()->CreateDepthStencilView(depthBuffer.Get(), &dsViewDesc, dsvHandle);
		dsvHandle.Offset(descSize);
	}
}

CD3DX12_CPU_DESCRIPTOR_HANDLE DepthStencilView::GetDsvHandle(const UINT bufferIndex) noexcept
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE bufferHandle;
	bufferHandle.InitOffsetted(dsvHeap->GetCPUDescriptorHandleForHeapStart(), bufferIndex, descSize);
	return bufferHandle;
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
	CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE{ dsvHeap->GetCPUDescriptorHandleForHeapStart() };
	for (UINT i = 0; i < texturesNum; ++i)
	{
		commandList->ClearDepthStencilView(dsvHandle, clearFlags, clearValue, 0, 0, nullptr);
		dsvHandle.Offset(descSize);
	}
}

ID3D12Resource* DepthStencilView::GetBuffer() noexcept
{
	return depthBuffer.Get();
}
