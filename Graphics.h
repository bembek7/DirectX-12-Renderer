#pragma once
#include "BetterWindows.h"
#include <initguid.h>
#include "d3dx12/d3dx12.h"
#include <wrl\client.h>
#include <DirectXMath.h>
#include <memory>
#include <dxgi1_6.h>
#include "VertexBuffer.h"
#include "Fence.h"
#include "RootSignature.h"

class Graphics
{
	friend class Bindable;
	friend class Fence;
public:
	Graphics(const HWND& hWnd, const float windowWidth, const float windowHeight);
	~Graphics() = default;
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;

	void OnUpdate();
	void OnRender();
	void OnDestroy();

	float GetWindowWidth() const noexcept;
	float GetWindowHeight() const noexcept;

	void SetProjection(const DirectX::XMFLOAT4X4 proj) noexcept;
	DirectX::XMMATRIX GetProjection() const noexcept;
	void SetCamera(const DirectX::XMMATRIX cam) noexcept;
	DirectX::XMMATRIX GetCamera() const noexcept;

	void ResetCommandListAndAllocator();
	void ExecuteCommandList();

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

	static constexpr UINT bufferCount = 2;

	std::unique_ptr<VertexBuffer> vertexBuffer;
	std::unique_ptr<RootSignature> rootSignature;

	// Pipeline objects.
	std::unique_ptr<Bindable> viewport;
	std::unique_ptr<Bindable> scissorRect;
	Microsoft::WRL::ComPtr<IDXGISwapChain3> swapChain;
	Microsoft::WRL::ComPtr<ID3D12Device2> device;
	Microsoft::WRL::ComPtr<ID3D12Resource> renderTargets[bufferCount];
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;
	UINT rtvDescriptorSize;

	// Synchronization objects.
	UINT curBackBufferIndex;
	std::unique_ptr<Fence> fence;
};
