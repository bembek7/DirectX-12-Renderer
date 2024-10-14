#pragma once
#include "d3dx12/d3dx12.h"
#include <wrl\client.h>
#include "Graphics.h"

class BufferLoader
{
public:
	template<typename T>
	static Microsoft::WRL::ComPtr<ID3D12Resource> GenerateBufferFromData(Graphics& graphics, const std::vector<T>& data, const D3D12_RESOURCE_STATES resourceState)
	{
		namespace Wrl = Microsoft::WRL;
		UINT dataNum = UINT(data.size());
		Wrl::ComPtr<ID3D12Resource> finalBuffer;
		{
			const CD3DX12_HEAP_PROPERTIES heapProps{ D3D12_HEAP_TYPE_DEFAULT };
			const auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(dataNum * sizeof(T));
			CHECK_HR(graphics.GetDevice()->CreateCommittedResource(
				&heapProps,
				D3D12_HEAP_FLAG_NONE,
				&resourceDesc,
				D3D12_RESOURCE_STATE_COMMON,
				nullptr, IID_PPV_ARGS(&finalBuffer)
			));
		}

		Wrl::ComPtr<ID3D12Resource> uploadBuffer;
		{
			const CD3DX12_HEAP_PROPERTIES heapProps{ D3D12_HEAP_TYPE_UPLOAD };
			const auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(dataNum * sizeof(T));
			CHECK_HR(graphics.GetDevice()->CreateCommittedResource(
				&heapProps,
				D3D12_HEAP_FLAG_NONE,
				&resourceDesc,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr, IID_PPV_ARGS(&uploadBuffer)
			));
		}
		// copy vector of data to upload buffer
		{
			T* mappedData = nullptr;
			CHECK_HR(uploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&mappedData)));
			std::memcpy(mappedData, data.data(), dataNum * sizeof(T));
			uploadBuffer->Unmap(0, nullptr);
		}

		graphics.WaitForSignal();
		graphics.ResetCommandListAndAllocator();
		// copy upload buffer to buffer
		graphics.GetMainCommandList()->CopyResource(finalBuffer.Get(), uploadBuffer.Get());

		const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			finalBuffer.Get(),
			D3D12_RESOURCE_STATE_COPY_DEST, resourceState);
		graphics.GetMainCommandList()->ResourceBarrier(1, &barrier);

		// close command list
		CHECK_HR(graphics.GetMainCommandList()->Close());

		graphics.ExecuteCommandList();

		graphics.Signal();

		return finalBuffer;
	}
};
