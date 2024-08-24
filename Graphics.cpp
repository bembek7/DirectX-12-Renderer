#include "Graphics.h"
#include "ThrowMacros.h"
#include <d3d11.h>
#include <d3dcommon.h>

Graphics::Graphics(const HWND& hWnd, const unsigned int windowWidth, const unsigned int windowHeight)
{
	DXGI_SWAP_CHAIN_DESC scd = {};
	scd.BufferDesc.Width = 0;
	scd.BufferDesc.Height = 0;
	scd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	scd.BufferDesc.RefreshRate.Numerator = 0;
	scd.BufferDesc.RefreshRate.Denominator = 0;
	scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	scd.SampleDesc.Count = 1;
	scd.SampleDesc.Quality = 0;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.BufferCount = 1;
	scd.OutputWindow = hWnd;
	scd.Windowed = TRUE;
	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
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

	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;
	CHECK_HR(device->CreateDepthStencilState(&depthStencilDesc, &depthStencilState));

	context->OMSetDepthStencilState(depthStencilState.Get(), 1u);

	D3D11_TEXTURE2D_DESC depthDesc = {};
	depthDesc.Width = windowWidth;
	depthDesc.Height = windowHeight;
	depthDesc.MipLevels = 1u;
	depthDesc.ArraySize = 1u;
	depthDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	depthDesc.SampleDesc.Count = 1u;
	depthDesc.SampleDesc.Quality = 0u;
	depthDesc.Usage = D3D11_USAGE_DEFAULT;
	depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencil;
	CHECK_HR(device->CreateTexture2D(&depthDesc, nullptr, &depthStencil));

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0u;

	CHECK_HR(device->CreateDepthStencilView(depthStencil.Get(), &depthStencilViewDesc, &depthStencilView));

	D3D11_TEXTURE2D_DESC depthSMDesc = {};
	depthSMDesc.Width = windowWidth;
	depthSMDesc.Height = windowHeight;
	depthSMDesc.MipLevels = 1u;
	depthSMDesc.ArraySize = 1u;
	depthSMDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	depthSMDesc.SampleDesc.Count = 1u;
	depthSMDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;

	CHECK_HR(device->CreateTexture2D(&depthSMDesc, nullptr, &shadowMap));

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewSMDesc = {};
	depthStencilViewSMDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthStencilViewSMDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewSMDesc.Texture2D.MipSlice = 0u;

	CHECK_HR(device->CreateDepthStencilView(shadowMap.Get(), &depthStencilViewSMDesc, &shadowRenderDepthView));

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = {};
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Format = DXGI_FORMAT_R32_FLOAT;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	CHECK_HR(device->CreateShaderResourceView(shadowMap.Get(), &shaderResourceViewDesc, &shaderResourceView));

	context->PSSetShaderResources(0u, 1u, shaderResourceView.GetAddressOf());

	D3D11_SAMPLER_DESC comparisonSamplerDesc = {};
	comparisonSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	comparisonSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	comparisonSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	comparisonSamplerDesc.BorderColor[0] = 1.0f;
	comparisonSamplerDesc.BorderColor[1] = 1.0f;
	comparisonSamplerDesc.BorderColor[2] = 1.0f;
	comparisonSamplerDesc.BorderColor[3] = 1.0f;
	comparisonSamplerDesc.MinLOD = 0.f;
	comparisonSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	comparisonSamplerDesc.MipLODBias = 0.f;
	comparisonSamplerDesc.MaxAnisotropy = 0;
	comparisonSamplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
	comparisonSamplerDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT;

	CHECK_HR(device->CreateSamplerState(&comparisonSamplerDesc, &comparisonSampler));

	context->PSSetSamplers(0u, 1u, comparisonSampler.GetAddressOf());

	D3D11_RASTERIZER_DESC drawingRenderStateDesc = {};
	drawingRenderStateDesc.CullMode = D3D11_CULL_BACK;
	drawingRenderStateDesc.FillMode = D3D11_FILL_SOLID;
	drawingRenderStateDesc.DepthClipEnable = true;
	CHECK_HR(device->CreateRasterizerState(&drawingRenderStateDesc, &drawingRenderState));

	D3D11_RASTERIZER_DESC shadowRenderStateDesc = {};
	shadowRenderStateDesc.CullMode = D3D11_CULL_FRONT;
	shadowRenderStateDesc.FillMode = D3D11_FILL_SOLID;
	shadowRenderStateDesc.DepthClipEnable = true;

	CHECK_HR(device->CreateRasterizerState(&shadowRenderStateDesc, &shadowRenderState));

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = (FLOAT)windowWidth;
	viewport.Height = (FLOAT)windowHeight;
	viewport.MaxDepth = 1.f;
	viewport.MinDepth = 0.f;

	shadowViewport.TopLeftX = 0;
	shadowViewport.TopLeftY = 0;
	shadowViewport.Width = (FLOAT)windowWidth;
	shadowViewport.Height = (FLOAT)windowHeight;
	shadowViewport.MaxDepth = 1.f;
	shadowViewport.MinDepth = 0.f;

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	const float screenRatio = viewport.Height / viewport.Width;
	SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, screenRatio, 0.5f, 30.0f));
}

void Graphics::DrawIndexed(const size_t numIndices) noexcept
{
	context->DrawIndexed((UINT)numIndices, 0u, 0);
}

void Graphics::BeginFrame() noexcept
{
	const float color[] = { 0.04f, 0.02f, 0.1f, 1.0f };
	context->ClearRenderTargetView(renderTargetView.Get(), color);
	context->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0u);
	context->ClearDepthStencilView(shadowRenderDepthView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u);
}

void Graphics::SetRenderTargetForShadowMap()
{
	context->OMSetRenderTargets(0u, nullptr, shadowRenderDepthView.Get());
	context->RSSetState(shadowRenderState.Get());
	context->RSSetViewports(1u, &shadowViewport);
}

void Graphics::SetNormalRenderTarget()
{
	const float color[] = { 0.04f, 0.02f, 0.1f, 1.0f };
	context->OMSetRenderTargets(1u, renderTargetView.GetAddressOf(), depthStencilView.Get());
	context->RSSetState(drawingRenderState.Get());
	context->PSSetShaderResources(0u, 1u, shaderResourceView.GetAddressOf());
	context->RSSetViewports(1u, &viewport);
}

void Graphics::EndFrame()
{
	CHECK_HR(swapChain->Present(1u, 0u));
}

void Graphics::SetProjection(const DirectX::XMMATRIX& proj) noexcept
{
	projection = proj;
}

DirectX::XMMATRIX Graphics::GetProjection() const noexcept
{
	return projection;
}

void Graphics::SetCamera(const DirectX::XMMATRIX& cam) noexcept
{
	camera = cam;
}

DirectX::XMMATRIX Graphics::GetCamera() const noexcept
{
	return camera;
}