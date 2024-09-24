#include "Graphics.h"
#include "ThrowMacros.h"
#include <d3d11.h>
#include <d3dcommon.h>
#include <numbers>
#include "Camera.h"
#include "DepthCubeTexture.h"

Graphics::Graphics(const HWND& hWnd, const float windowWidth, const float windowHeight) :
	windowWidth(windowWidth),
	windowHeight(windowHeight)
{
	DXGI_SWAP_CHAIN_DESC scd = {};
	scd.BufferDesc.Width = 0u;
	scd.BufferDesc.Height = 0u;
	scd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	scd.BufferDesc.RefreshRate.Numerator = 0u;
	scd.BufferDesc.RefreshRate.Denominator = 0u;
	scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	scd.SampleDesc.Count = 1u;
	scd.SampleDesc.Quality = 0;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.BufferCount = 2u;
	scd.OutputWindow = hWnd;
	scd.Windowed = TRUE;
	scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	scd.Flags = 0;

	UINT swapCreateFlags = 0u;
#ifndef NDEBUG
	swapCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	CHECK_HR(D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		swapCreateFlags,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&scd,
		&swapChain,
		&device,
		nullptr,
		&context
	));

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	gui = std::make_unique<Gui>(hWnd, device.Get(), context.Get());
}

void Graphics::DrawIndexed(const size_t numIndices) noexcept
{
	context->DrawIndexed((UINT)numIndices, 0u, 0);
}

void Graphics::BeginFrame() noexcept
{
	gui->BeginFrame();
}

void Graphics::EndFrame()
{
	gui->EndFrame();

	CHECK_HR(swapChain->Present(1u, 0u));
}

Gui* const Graphics::GetGui() noexcept
{
	return gui.get();
}

void Graphics::SetCurrentRenderTargetView(Microsoft::WRL::ComPtr<ID3D11RenderTargetView> newCurrRTV) noexcept
{
	currentRenderTargetView = newCurrRTV;
}

void Graphics::SetCurrentDepthStenilView(Microsoft::WRL::ComPtr<ID3D11DepthStencilView> newCurrDSV) noexcept
{
	currentDepthStencilView = newCurrDSV;
}

void Graphics::BindCurrentRenderTarget()
{
	UINT numViews = 0u;
	if (currentRenderTargetView)
	{
		numViews = 1u;
	}
	context->OMSetRenderTargets(numViews, currentRenderTargetView.GetAddressOf(), currentDepthStencilView.Get());
}

void Graphics::ClearRenderTargetBinds()
{
	context->OMSetRenderTargets(0, nullptr, nullptr);
}

float Graphics::GetWindowWidth() const noexcept
{
	return windowWidth;
}

float Graphics::GetWindowHeight() const noexcept
{
	return windowHeight;
}

void Graphics::SetProjection(const DirectX::XMFLOAT4X4 proj) noexcept
{
	projection = proj;
}

DirectX::XMMATRIX Graphics::GetProjection() const noexcept
{
	return DirectX::XMLoadFloat4x4(&projection);
}

void Graphics::SetCamera(const DirectX::XMMATRIX cam) noexcept
{
	DirectX::XMStoreFloat4x4(&camera, cam);
}

DirectX::XMMATRIX Graphics::GetCamera() const noexcept
{
	return DirectX::XMLoadFloat4x4(&camera);
}