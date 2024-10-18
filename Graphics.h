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

	ID3D12GraphicsCommandList* GetMainCommandList();
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> CreateBundle();
	ID3D12Device2* GetDevice();

	PipelineState::PipelineStateStream GetCommonPSS();
	RootSignature* GetRootSignature();
	UINT GetCbvSrvDescriptorSize() const noexcept;

	float GetWindowWidth() const noexcept;
	float GetWindowHeight() const noexcept;

	std::vector<CD3DX12_ROOT_PARAMETER>& GetCommonRootParametersRef() noexcept;

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

private:
	void LoadPipeline(const HWND& hWnd);
	void LoadAssets();
	void CreateRootSignature();
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
	std::vector<D3D12_DESCRIPTOR_RANGE> texesDescRanges;
	std::unique_ptr<RootSignature> rootSignature;
	std::unique_ptr<PipelineState> pipelineState;

	// Pipeline objects.
	Microsoft::WRL::ComPtr<IDXGISwapChain3> swapChain;
	Microsoft::WRL::ComPtr<ID3D12Device2> device;
	Microsoft::WRL::ComPtr<ID3D12Resource> renderTargets[bufferCount];
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocators[bufferCount];
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> bundleAllocator;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue;
	Microsoft::WRL::ComPtr<ID3D12Resource> depthBuffer;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvHeap;
	CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;
	UINT rtvDescriptorSize;

	// Synchronization objects.
	UINT curBufferIndex = 0;
	HANDLE fenceEvent;
	Microsoft::WRL::ComPtr<ID3D12Fence> fence;
	UINT64 fenceValues[bufferCount] = { 0 };
};
