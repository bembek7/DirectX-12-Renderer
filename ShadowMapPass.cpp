#include "ShadowMapPass.h"
#include "Viewport.h"
#include "Graphics.h"
#include "ScissorRectangle.h"
#include "Actor.h"
#include "Camera.h"
#include "DirectionalLight.h"

namespace Dx = DirectX;

ShadowMapPass::ShadowMapPass(Graphics& graphics)
{
	/*const float shadowMapCubeFaceSize = float(shadowMapCube->GetFaceSize());
	bindables.push_back(std::make_unique<Viewport>(shadowMapCubeFaceSize, shadowMapCubeFaceSize));
	bindables.push_back(std::make_unique<NullRenderTargetView>());
	bindables.push_back(std::make_unique<DepthStencilState>(graphics, DepthStencilState::Usage::Regular));
	auto& bindablesPool = BindablesPool::GetInstance();
	sharedBindables.push_back(bindablesPool.GetBindable<ShadowRasterizer>(graphics, D3D11_CULL_FRONT));
	bindables.push_back(std::make_unique<NullPixelShader>());

	DirectX::XMStoreFloat4x4(&projection, DirectX::XMMatrixPerspectiveFovLH(float(std::numbers::pi) / 2.f, 1.0f, 0.5f, 200.0f));*/

	const float windowWidth = graphics.GetWindowWidth();
	const float windowHeight = graphics.GetWindowHeight();

	bindables.push_back(std::make_unique<ScissorRectangle>());
	bindables.push_back(std::make_unique<Viewport>(windowWidth, windowHeight));

	depthStencilView = std::make_unique<DepthStencilView>(graphics, DepthStencilView::Usage::Depth, 1.f, UINT(windowWidth), UINT(windowHeight));

	//depthStencilView = std::make_unique<DepthStencilView>(graphics, DepthStencilView::Usage::Depth, 0.f, UINT(windowWidth), UINT(windowHeight));

	Dx::XMStoreFloat4x4(&projection, Dx::XMMatrixOrthographicLH(300.0f, 300 * windowHeight / windowWidth, 0.5f, 700.0f));
}

void ShadowMapPass::Execute(Graphics& graphics, const std::vector<std::unique_ptr<Actor>>& actors, const std::vector<Light*>& lights, const DirectionalLight* const directionalLight)
{
	Pass::Execute(graphics);

	const auto lightDir = directionalLight->GetActorForwardVector();
	const auto lightLocation = Dx::XMVectorScale(lightDir, -300);
	graphics.SetCamera(Dx::XMMatrixLookAtLH(lightLocation, Dx::XMVectorAdd(lightLocation, lightDir), directionalLight->GetActorUpVector()));

	for (auto& actor : actors)
	{
		actor->Update(graphics);
	}
	graphics.ClearRenderTargetView();
	depthStencilView->Clear(graphics.GetMainCommandList());
	auto rtv = graphics.GetRtvCpuHandle();
	auto dsvHandle = depthStencilView->GetDsvHandle();
	graphics.GetMainCommandList()->OMSetRenderTargets(1, &rtv, TRUE, &dsvHandle);

	for (auto& actor : actors)
	{
		actor->Draw(graphics, lights);
	}
	/*shadowMapCube->Clear(graphics);

	static const std::vector<DirectX::XMFLOAT3> shadowCameraRotations =
	{
		{0.f, 90.f, 0.f},
		{0.f, 270.f, 0.f},
		{270.f, 0.0f, 0.f},
		{90.f, 0.f, 0.f},
		{0.f, 0.f, 0.f},
		{0.f, 180.f, 0.f},
	};
	for (unsigned int faceIndex = 0; faceIndex < 6; faceIndex++)
	{
		pointLight->SetActorRotation(shadowCameraRotations[faceIndex]);
		graphics.SetCamera(pointLight->GetLightPerspective());
		//graphics.SetCurrentDepthStenilView(shadowMapCube->GetDepthBuffer(faceIndex)->Get());
		//graphics.BindCurrentRenderTarget();

		for (auto& actor : actors)
		{
			actor->RenderShadowMap(graphics);
		}
	}
	pointLight->SetActorRotation(DirectX::XMFLOAT3{ 0.f, 0.f, 0.f });
	//graphics.ClearRenderTargetBinds();*/
}