#include "Graphics.h"
#include "ThrowMacros.h"
#include <d3d11.h>
#include <d3dcommon.h>
#include "BindablesPool.h"
#include "ShadowRasterizer.h"

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

	D3D11_TEXTURE2D_DESC depthTexDesc = {};
	depthTexDesc.Width = windowWidth;
	depthTexDesc.Height = windowHeight;
	depthTexDesc.MipLevels = 1u;
	depthTexDesc.ArraySize = 1u;
	depthTexDesc.SampleDesc.Count = 1u;
	depthTexDesc.SampleDesc.Quality = 0u;
	depthTexDesc.Usage = D3D11_USAGE_DEFAULT;

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0u;

	depthTexDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	depthTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilView = std::make_unique<DepthStencilView>(device.Get(), depthTexDesc, depthStencilViewDesc);

	depthTexDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	depthTexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
	depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	shadowMapDepthStencilView = std::make_unique<DepthStencilView>(device.Get(), depthTexDesc, depthStencilViewDesc);

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = {};
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Format = DXGI_FORMAT_R32_FLOAT;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	CHECK_HR(device->CreateShaderResourceView(shadowMapDepthStencilView->GetTexture(), &shaderResourceViewDesc, &shadowMap));

	auto& bindablesPool = BindablesPool::GetInstance();

	comparisonSampler = bindablesPool.GetBindable<Sampler>(*this, 0u, Sampler::Mode::Comparison);

	comparisonSampler->Bind(*this);

	shadowMapRasterizer = bindablesPool.GetBindable<ShadowRasterizer>(*this, D3D11_CULL_FRONT);

	drawingViewport = std::make_unique<Viewport>(float(windowWidth), float(windowHeight));
	shadowViewport = std::make_unique<Viewport>(float(windowWidth), float(windowHeight));

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	const float screenRatio = float(windowHeight) / windowWidth;
	SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, screenRatio, 0.5f, 400.0f));

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
	context->ClearDepthStencilView(depthStencilView->Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0u);
	context->ClearDepthStencilView(shadowMapDepthStencilView->Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u);

	gui->BeginFrame();
}

void Graphics::SetRenderTargetForShadowMap()
{
	writeMaskDepthStencilState->Bind(*this);
	context->OMSetRenderTargets(0u, nullptr, shadowMapDepthStencilView->Get());
	shadowMapRasterizer->Bind(*this);
	shadowViewport->Bind(*this);
}

void Graphics::SetNormalRenderTarget()
{
	writeMaskDepthStencilState->Bind(*this);
	context->OMSetRenderTargets(1u, renderTargetView.GetAddressOf(), depthStencilView->Get());
	context->PSSetShaderResources(0u, 1u, shadowMap.GetAddressOf());
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

void Graphics::SetCamera(const DirectX::XMMATRIX cam) noexcept
{
	DirectX::XMStoreFloat4x4(&camera, cam);
}

DirectX::XMMATRIX Graphics::GetCamera() const noexcept
{
	return DirectX::XMLoadFloat4x4(&camera);
}