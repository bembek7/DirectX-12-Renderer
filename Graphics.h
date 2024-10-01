#pragma once
#include "BetterWindows.h"
#include <initguid.h>
#include "d3dx12/d3dx12.h"
#include <wrl\client.h>
#include <DirectXMath.h>
#include <memory>
#include <dxgi1_6.h>

class Graphics
{
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

private:
	void LoadPipeline(const HWND& hWnd);
	void LoadAssets();
	void PopulateCommandList();
	void WaitForPreviousFrame();

private:
	float windowWidth;
	float windowHeight;
	DirectX::XMFLOAT4X4 camera;
	DirectX::XMFLOAT4X4 projection;

	static constexpr UINT bufferCount = 2;

	// Pipeline objects.
	D3D12_VIEWPORT viewport;
	D3D12_RECT scissorRect;
	Microsoft::WRL::ComPtr<IDXGISwapChain3> swapChain;
	Microsoft::WRL::ComPtr<ID3D12Device2> device;
	Microsoft::WRL::ComPtr<ID3D12Resource> renderTargets[bufferCount];
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;
	UINT rtvDescriptorSize;

	struct Vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT4 color;
	};

	// App resources.
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;

	UINT nVertices;
	// Synchronization objects.
	UINT curBackBufferIndex;
	HANDLE fenceEvent;
	Microsoft::WRL::ComPtr<ID3D12Fence> fence;
	UINT64 fenceValue;
};
