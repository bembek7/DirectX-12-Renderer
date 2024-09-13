#pragma once
#include "BetterWindows.h"
#include <dxgi.h>
#include <d3d11.h>
#include <wrl\client.h>
#include <DirectXMath.h>
#include "DepthStencilView.h"
#include <memory>
#include "Gui.h"
#include "Sampler.h"
#include "Rasterizer.h"
#include "DepthStencilState.h"
#include "Viewport.h"
#include "DepthCubeTexture.h"

class Graphics
{
	friend class Bindable;
public:
	Graphics(const HWND& hWnd, const unsigned int windowWidth, const unsigned int windowHeight);
	~Graphics() = default;
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;

	void BeginFrame() noexcept;
	void SetRenderTargetForShadowMap(const unsigned int face);
	void SetNormalRenderTarget();
	void EndFrame();
	Gui* const GetGui() noexcept;

	void SetProjection(const DirectX::XMMATRIX proj) noexcept;
	DirectX::XMMATRIX GetProjection() const noexcept;
	DirectX::XMMATRIX GetShadowMappingProjection() const noexcept;
	void SetCamera(const DirectX::XMMATRIX cam) noexcept;
	DirectX::XMMATRIX GetCamera() const noexcept;

	void DrawIndexed(const size_t numIndices) noexcept;

private:
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;
	std::shared_ptr<Bindable> comparisonSampler;
	std::shared_ptr<Bindable> shadowMapRasterizer;
	std::unique_ptr<DepthStencilState> writeMaskDepthStencilState;
	std::unique_ptr<DepthStencilView> depthStencilView;
	std::unique_ptr<DepthCubeTexture> shadowMapCube;
	std::unique_ptr<Viewport> drawingViewport;
	std::unique_ptr<Viewport> shadowViewport;

	DirectX::XMFLOAT4X4 camera;
	DirectX::XMFLOAT4X4 projection;
	DirectX::XMFLOAT4X4 drawingProjection;
	DirectX::XMFLOAT4X4 shadowMappingProjection;
	std::unique_ptr<Gui> gui;
};
