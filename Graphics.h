#pragma once
#include "BetterWindows.h"
#include <initguid.h>
#include "d3dx12/d3dx12.h"
#include <wrl\client.h>
#include <DirectXMath.h>
#include <memory>
#include <dxgi1_6.h>
#include "Fence.h"
#include "RootSignature.h"
#include "ThrowMacros.h"
#include "Gui.h"

class Graphics
{
	friend class Bindable;
	friend class Fence;
	friend class TexLoader;
	friend class Texture;
	friend class Material;
public:
	Graphics(const HWND& hWnd, const float windowWidth, const float windowHeight);
	~Graphics() = default;
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;

	void RenderBegin();
	void RenderEnd();
	void OnDestroy();

	void DrawIndexed(const UINT indicesNumber);
	void BindLighting();
	void SetLight(PointLight* const pointLight) noexcept;

	CD3DX12_CPU_DESCRIPTOR_HANDLE GetCbvSrvCpuHandle() const noexcept;
	CD3DX12_GPU_DESCRIPTOR_HANDLE GetCbvSrvGpuHeapStartHandle() const noexcept;
	UINT GetCbvSrvDescriptorSize() const noexcept;
	void OffsetCbvSrvCpuHandle(INT descNum);
	float GetWindowWidth() const noexcept;
	float GetWindowHeight() const noexcept;
	DXGI_FORMAT GetRTFormat() const noexcept;
	std::vector<CD3DX12_ROOT_PARAMETER>& GetCommonRootParametersRef() noexcept;

	void SetProjection(const DirectX::XMMATRIX proj) noexcept;
	DirectX::XMMATRIX GetProjection() const noexcept;
	void SetCamera(const DirectX::XMMATRIX cam) noexcept;
	DirectX::XMMATRIX GetCamera() const noexcept;

	void ResetCommandListAndAllocator();
	void ExecuteCommandList();

	Gui* const GetGui() noexcept;

	template<typename T>
	Microsoft::WRL::ComPtr<ID3D12Resource> GenerateBufferFromData(const std::vector<T>& data)
	{
		namespace Wrl = Microsoft::WRL;
		UINT dataNum = UINT(data.size());
		Wrl::ComPtr<ID3D12Resource> finalBuffer;
		{
			const CD3DX12_HEAP_PROPERTIES heapProps{ D3D12_HEAP_TYPE_DEFAULT };
			const auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(dataNum * sizeof(T));
			CHECK_HR(device->CreateCommittedResource(
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
			CHECK_HR(device->CreateCommittedResource(
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

		ResetCommandListAndAllocator();
		// copy upload buffer to buffer
		commandList->CopyResource(finalBuffer.Get(), uploadBuffer.Get());

		// close command list
		CHECK_HR(commandList->Close());

		ExecuteCommandList();

		fence->WaitForQueueFinish(*this, INFINITE);

		return finalBuffer;
	}

private:
	void LoadPipeline(const HWND& hWnd);
	void LoadAssets();
	void PopulateCommandList();
	void ClearRenderTarget(ID3D12Resource* const backBuffer, const CD3DX12_CPU_DESCRIPTOR_HANDLE& rtv);

private:
	float windowWidth;
	float windowHeight;
	DirectX::XMFLOAT4X4 camera;
	DirectX::XMFLOAT4X4 projection;
	UINT cbvSrvDescriptorSize = 0;
	static constexpr UINT bufferCount = 2;
	static constexpr DXGI_FORMAT renderTargetDxgiFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

	std::unique_ptr<Gui> gui;

	std::vector<CD3DX12_ROOT_PARAMETER> commonRootParameters;

	PointLight* light = nullptr;

	// Pipeline objects.
	std::unique_ptr<Bindable> viewport;
	std::unique_ptr<Bindable> scissorRect;
	Microsoft::WRL::ComPtr<IDXGISwapChain3> swapChain;
	Microsoft::WRL::ComPtr<ID3D12Device2> device;
	Microsoft::WRL::ComPtr<ID3D12Resource> renderTargets[bufferCount];
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue;
	Microsoft::WRL::ComPtr<ID3D12Resource> depthBuffer;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvHeap;
	CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvHeap;
	CD3DX12_CPU_DESCRIPTOR_HANDLE srvCpuHandle;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;
	UINT rtvDescriptorSize;

	// Synchronization objects.
	UINT curBackBufferIndex;
	std::unique_ptr<Fence> fence;
};
