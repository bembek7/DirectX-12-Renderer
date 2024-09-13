#include "Graphics.h"
#include "ThrowMacros.h"
#include <d3d11.h>
#include <d3dcommon.h>
#include "BindablesPool.h"
#include "ShadowRasterizer.h"
#include <numbers>

Graphics::Graphics(const HWND& hWnd, const unsigned int windowWidth, const unsigned int windowHeight)
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

	Microsoft::WRL::ComPtr<ID3D11Resource> backBuffer;
	CHECK_HR(swapChain->GetBuffer(0u, __uuidof(ID3D11Resource), &backBuffer));
	CHECK_HR(device->CreateRenderTargetView(backBuffer.Get(), nullptr, &renderTargetView));

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
	depthStencilDesc.DepthEnable = TRUE;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	writeMaskDepthStencilState = std::make_unique<DepthStencilState>(*this, depthStencilDesc);

	depthStencilView = std::make_unique<DepthStencilView>(device.Get(), DepthStencilView::Usage::DepthStencil, windowWidth, windowHeight);

	const float shadowMapCubeFaceSize = 1024.f;
	shadowMapCube = std::make_unique<DepthCubeTexture>(*this, 0, (UINT)shadowMapCubeFaceSize);

	auto& bindablesPool = BindablesPool::GetInstance();

	comparisonSampler = bindablesPool.GetBindable<Sampler>(*this, 0u, Sampler::Mode::Comparison);

	comparisonSampler->Bind(*this);

	shadowMapRasterizer = bindablesPool.GetBindable<ShadowRasterizer>(*this, D3D11_CULL_FRONT);

	shadowViewport = std::make_unique<Viewport>(shadowMapCubeFaceSize, shadowMapCubeFaceSize);
	drawingViewport = std::make_unique<Viewport>(float(windowWidth), float(windowHeight));

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	DirectX::XMStoreFloat4x4(&drawingProjection, DirectX::XMMatrixPerspectiveLH(1.0f, float(windowHeight) / windowWidth, 0.5f, 200.0f));
	DirectX::XMStoreFloat4x4(&shadowMappingProjection, DirectX::XMMatrixPerspectiveFovLH(float(std::numbers::pi) / 2.f, 1.0f, 0.5f, 200.0f));

	gui = std::make_unique<Gui>(hWnd, device.Get(), context.Get());
}

void Graphics::DrawIndexed(const size_t numIndices) noexcept
{
	context->DrawIndexed((UINT)numIndices, 0u, 0);
}

void Graphics::BeginFrame() noexcept
{
	const float color[] = { 0.04f, 0.02f, 0.1f, 1.0f };
	context->ClearRenderTargetView(renderTargetView.Get(), color);
	depthStencilView->Clear(context.Get());
	for (unsigned int i = 0; i < 6; i++)
	{
		shadowMapCube->GetDepthBuffer(i)->Clear(context.Get());
	}

	gui->BeginFrame();
}

void Graphics::SetRenderTargetForShadowMap(const unsigned int face)
{
	projection = shadowMappingProjection;
	writeMaskDepthStencilState->Bind(*this);
	context->OMSetRenderTargets(0u, nullptr, shadowMapCube->GetDepthBuffer(face)->Get());
	shadowMapRasterizer->Bind(*this);
	shadowViewport->Bind(*this);
}

void Graphics::SetNormalRenderTarget()
{
	projection = drawingProjection;
	writeMaskDepthStencilState->Bind(*this);
	context->OMSetRenderTargets(1u, renderTargetView.GetAddressOf(), depthStencilView->Get());
	shadowMapCube->Bind(*this);
	drawingViewport->Bind(*this);
}

void Graphics::EndFrame()
{
	gui->EndFrame();

	CHECK_HR(swapChain->Present(1u, 0u));
	ID3D11ShaderResourceView* const nullSRV = nullptr;
	context->PSSetShaderResources(0, 1, &nullSRV);
}

Gui* const Graphics::GetGui() noexcept
{
	return gui.get();
}

void Graphics::SetProjection(const DirectX::XMMATRIX proj) noexcept
{
	DirectX::XMStoreFloat4x4(&projection, proj);
}

DirectX::XMMATRIX Graphics::GetProjection() const noexcept
{
	return DirectX::XMLoadFloat4x4(&projection);
}

DirectX::XMMATRIX Graphics::GetShadowMappingProjection() const noexcept
{
	return DirectX::XMLoadFloat4x4(&shadowMappingProjection);
}

void Graphics::SetCamera(const DirectX::XMMATRIX cam) noexcept
{
	DirectX::XMStoreFloat4x4(&camera, cam);
}

DirectX::XMMATRIX Graphics::GetCamera() const noexcept
{
	return DirectX::XMLoadFloat4x4(&camera);
}