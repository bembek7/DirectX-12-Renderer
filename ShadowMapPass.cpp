#include "ShadowMapPass.h"
//#include "BindablesPool.h"
//#include <numbers>
//#include "ShadowRasterizer.h"
//#include <DirectXMath.h>
#include "Viewport.h"
//#include "DepthStencilState.h"
//#include "PixelShader.h"
//#include "RenderTargetView.h"
//#include "Actor.h"
#include "Graphics.h"
//#include "PointLight.h"
//#include "DepthCubeTexture.h"
#include "ScissorRectangle.h"

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

	DirectX::XMMATRIX reverseZ =
	{
		1.0f, 0.0f,  0.0f, 0.0f,
		0.0f, 1.0f,  0.0f, 0.0f,
		0.0f, 0.0f, -1.0f, 0.0f,
		0.0f, 0.0f,  1.0f, 1.0f
	};

	DirectX::XMStoreFloat4x4(&projection, DirectX::XMMatrixOrthographicLH(1.0f, windowHeight / windowWidth, 0.5f, 200.0f) * reverseZ);
}

void ShadowMapPass::Execute(Graphics& graphics, const std::vector<std::shared_ptr<Actor>>& actors, PointLight* const pointLight)
{
	Pass::Execute(graphics);

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