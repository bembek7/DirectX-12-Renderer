#include "RayTracingPass.h"
#include "Graphics.h"
#include "Viewport.h"

RayTracingPass::RayTracingPass(Graphics& graphics) :
	Pass(graphics, PassType::RayTracingPass)
{
	const float windowWidth = graphics.GetWindowWidth();
	const float windowHeight = graphics.GetWindowHeight();
	bindables.push_back(std::make_unique<Viewport>(windowWidth, windowHeight));
}

void RayTracingPass::Execute(Graphics& graphics)
{
	Pass::Execute(graphics);
	graphics.ClearRenderTargetView();
}