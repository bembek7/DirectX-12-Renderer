#pragma once
#include "BetterWindows.h"
#include <initguid.h>
#include "d3dx12/d3dx12.h"
#include <wrl\client.h>
#include <DirectXMath.h>
#include <memory>
#include <dxgi1_6.h>
#include "ThrowMacros.h"
#include "Gui.h"
#include "PipelineState.h"
#include "RootSignature.h"
#include "Pass.h"
#include <array>

class Light;

class Graphics
{
public:
	Graphics(const HWND& hWnd, const float windowWidth, const float windowHeight);
	~Graphics() = default;
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;

	void RenderBegin();
	void RenderEnd();
	void OnDestroy();

	void ExecuteBundle(ID3D12GraphicsCommandList* const bundle);
	void ClearRenderTargetView();
	CD3DX12_CPU_DESCRIPTOR_HANDLE GetRtvCpuHandle() const noexcept;

	ID3D12GraphicsCommandList* GetMainCommandList();
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> CreateBundle();
	ID3D12Device2* GetDevice();

	UINT GetCbvSrvDescriptorSize() const noexcept;
	UINT GetDsvDescriptorSize() const noexcept;

	float GetWindowWidth() const noexcept;
	float GetWindowHeight() const noexcept;

	void SetProjection(const DirectX::XMMATRIX proj) noexcept;
	void SetProjection(const DirectX::XMFLOAT4X4 proj) noexcept;
	DirectX::XMMATRIX GetProjection() const noexcept;
	void SetCamera(const DirectX::XMMATRIX cam) noexcept;
	DirectX::XMMATRIX GetCamera() const noexcept;

	void ResetCommandListAndAllocator();
	void ExecuteCommandList();
	void WaitForQueueFinish();
	void Signal();
	void WaitForSignal();

	Gui* const GetGui() noexcept;

	void CreateSRV(ID3D12Resource* const resource, const CD3DX12_CPU_DESCRIPTOR_HANDLE& srvCpuHandle);

private:
	void LoadPipeline(const HWND& hWnd);
	void LoadAssets();

public:
	static constexpr DXGI_FORMAT renderTargetDxgiFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	static constexpr std::array<FLOAT, 4> clearColor4 = { 0.13f, 0.05f, 0.05f, 1.0f };
	static constexpr std::array<FLOAT, 3> clearColor3 = { 0.13f, 0.05f, 0.05f };

private:
	float windowWidth;
	float windowHeight;
	DirectX::XMFLOAT4X4 camera;
	DirectX::XMFLOAT4X4 projection;
	UINT cbvSrvDescriptorSize = 0;
	UINT dsvDescriptorSize = 0;
	static constexpr UINT bufferCount = 2;

	std::unique_ptr<Gui> gui;

	std::vector<CD3DX12_ROOT_PARAMETER> commonRootParameters;
	std::vector<D3D12_DESCRIPTOR_RANGE> texesDescRanges;

	// Pipeline objects.
	Microsoft::WRL::ComPtr<IDXGISwapChain3> swapChain;
	Microsoft::WRL::ComPtr<ID3D12Device2> device;
	Microsoft::WRL::ComPtr<ID3D12Resource> renderTargets[bufferCount];
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocators[bufferCount];
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> bundleAllocator;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;
	UINT rtvDescriptorSize;

	CD3DX12_CPU_DESCRIPTOR_HANDLE dethPrePassDSVHandle;

	// Synchronization objects.
	UINT curBufferIndex = 0;
	HANDLE fenceEvent;
	Microsoft::WRL::ComPtr<ID3D12Fence> fence;
	UINT64 fenceValues[bufferCount] = { 0 };
};
