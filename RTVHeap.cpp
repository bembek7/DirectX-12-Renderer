#include "RTVHeap.h"
#include "ThrowMacros.h"
#include "Graphics.h"

RTVHeap::RTVHeap(Graphics& graphics, const UINT buffersNum, const D3D12_RENDER_TARGET_VIEW_DESC* const rtvDescs)
{
	const D3D12_DESCRIPTOR_HEAP_DESC desc = {
		.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
		.NumDescriptors = buffersNum,
	};
	CHECK_HR(graphics.GetDevice()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&rtvHeap)));

    rtvDescriptorSize = graphics.GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	renderTargets.resize(buffersNum);
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart());
	for (UINT i = 0; i < buffersNum; i++)
	{
		// Create or initialize the render target resource here
		auto resourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(
			rtvDescs[i].Format,
			(UINT)graphics.GetWindowWidth(),
			(UINT)graphics.GetWindowHeight(),
			1, 1, 1, 0,
			D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET
		);

		D3D12_CLEAR_VALUE clearValue = {};
		clearValue.Format = rtvDescs[i].Format;
		clearValue.Color[0] = 0.0f;
		clearValue.Color[1] = 0.0f;
		clearValue.Color[2] = 0.0f;
		clearValue.Color[3] = 1.0f;

		auto heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		CHECK_HR(graphics.GetDevice()->CreateCommittedResource(
			&heapProperties,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			&clearValue,
			IID_PPV_ARGS(&renderTargets[i])
		));

		graphics.GetDevice()->CreateRenderTargetView(renderTargets[i].Get(), &rtvDescs[i], rtvHandle);
		rtvHandle.Offset(rtvDescriptorSize);
	}
}

CD3DX12_CPU_DESCRIPTOR_HANDLE RTVHeap::GetCPUHandle(const INT bufferIndex) noexcept
{
	return CD3DX12_CPU_DESCRIPTOR_HANDLE { rtvHeap->GetCPUDescriptorHandleForHeapStart(), bufferIndex, rtvDescriptorSize };
}
