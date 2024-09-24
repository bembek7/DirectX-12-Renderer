#pragma once
#include "BetterWindows.h"
#include <dxgi.h>
#include <d3d11.h>
#include <wrl\client.h>
#include <DirectXMath.h>
#include <memory>
#include "Gui.h"
#include "ShadowMapPass.h"
#include "RegularDrawingPass.h"

class Camera;

class Graphics
{
	friend class Bindable;
public:
	Graphics(const HWND& hWnd, const float windowWidth, const float windowHeight);
	~Graphics() = default;
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;

	void BeginFrame() noexcept;
	void Draw(const std::vector<std::shared_ptr<Actor>>& actors, const std::shared_ptr<PointLight>& pointLight, const Camera* const mainCamera);
	void EndFrame();
	Gui* const GetGui() noexcept;

	void SetCurrentRenderTargetView(Microsoft::WRL::ComPtr<ID3D11RenderTargetView> newCurrRTV) noexcept;
	void SetCurrentDepthStenilView(Microsoft::WRL::ComPtr<ID3D11DepthStencilView> newCurrDSV) noexcept;

	void BindCurrentRenderTarget();
	void ClearRenderTargetBinds();

	void SetProjection(const DirectX::XMFLOAT4X4 proj) noexcept;
	DirectX::XMMATRIX GetProjection() const noexcept;
	void SetCamera(const DirectX::XMMATRIX cam) noexcept;
	DirectX::XMMATRIX GetCamera() const noexcept;

	void DrawIndexed(const size_t numIndices) noexcept;

private:
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> currentRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> currentDepthStencilView;
	std::unique_ptr<ShadowMapPass> shadowPass;
	std::unique_ptr<RegularDrawingPass> drawingPass;

	DirectX::XMFLOAT4X4 camera;
	DirectX::XMFLOAT4X4 projection;
	std::unique_ptr<Gui> gui;
};
