#pragma once
#include "BetterWindows.h"
#include <dxgi.h>
#include <d3d11.h>
#include <wrl\client.h>
#include <DirectXMath.h>
#include "DepthStencilView.h"
#include <memory>

class Graphics
{
	friend class Bindable;
public:
	Graphics(const HWND& hWnd, const unsigned int windowWidth, const unsigned int windowHeight);
	~Graphics() = default;
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;

	void BeginFrame() noexcept;
	void SetRenderTargetForShadowMap();
	void SetNormalRenderTarget();
	void EndFrame();

	void SetProjection(const DirectX::XMMATRIX& proj) noexcept;
	DirectX::XMMATRIX GetProjection() const noexcept;
	void SetCamera(const DirectX::XMMATRIX& cam) noexcept;
	DirectX::XMMATRIX GetCamera() const noexcept;

	void DrawIndexed(const size_t numIndices) noexcept;

private:
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> comparisonSampler;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> drawingRenderState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> shadowRenderState;
	std::unique_ptr<DepthStencilView> depthStencilView;
	std::unique_ptr<DepthStencilView> shadowMapDepthStencilView;
	D3D11_VIEWPORT viewport{};
	D3D11_VIEWPORT shadowViewport{};

	DirectX::XMMATRIX camera;
	DirectX::XMMATRIX projection;
};
