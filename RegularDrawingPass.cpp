#include "RegularDrawingPass.h"
#include "Graphics.h"
#include "RenderTargetView.h"
#include "BindablesPool.h"
#include "Actor.h"
#include "Camera.h"
#include "PointLight.h"
#include "Viewport.h"
#include "DepthStencilState.h"
#include "DepthStencilView.h"
#include "Sampler.h"
#include "DepthCubeTexture.h"

RegularDrawingPass::RegularDrawingPass(Graphics& graphics, const float windowWidth, const float windowHeight, std::shared_ptr<DepthCubeTexture> shadowMapCube)
{
	bindables.push_back(std::make_unique<Viewport>(windowWidth, windowHeight));
	bindables.push_back(std::make_unique<RenderTargetView>(graphics));
	bindables.push_back(std::make_unique<DepthStencilView>(graphics, DepthStencilView::Usage::DepthStencil, UINT(windowWidth), UINT(windowHeight)));
	bindables.push_back(std::make_unique<DepthStencilState>(graphics, DepthStencilState::Usage::Regular));
	sharedBindables.push_back(std::move(shadowMapCube));

	auto& bindablesPool = BindablesPool::GetInstance();
	sharedBindables.push_back(bindablesPool.GetBindable<Sampler>(graphics, 0u, Sampler::Mode::Comparison));

	DirectX::XMStoreFloat4x4(&projection, DirectX::XMMatrixPerspectiveLH(1.0f, windowHeight / windowWidth, 0.5f, 200.0f));
}

void RegularDrawingPass::Execute(Graphics& graphics, const std::vector<std::shared_ptr<Actor>>& actors, const std::shared_ptr<PointLight>& pointLight, const Camera* const mainCamera)
{
	Pass::Execute(graphics);
	graphics.BindCurrentRenderTarget();
	graphics.SetCamera(mainCamera->GetMatrix());
	pointLight->Bind(graphics);
	for (auto& actor : actors)
	{
		actor->Draw(graphics);
	}
}