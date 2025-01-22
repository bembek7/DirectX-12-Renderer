#include "Pass.h"
#include "Graphics.h"
#include "Camera.h"
#include "ScissorRectangle.h"
#include "Viewport.h"


Pass::Pass(Graphics& graphics, PassType type, const std::vector<RPD::CBTypes>& constantBuffers, const std::vector<RPD::TextureTypes>& textures, const std::vector<RPD::SamplerTypes>& samplers) :
	type(type)
{
	rootSignature = std::make_unique<RootSignature>(graphics, constantBuffers, textures, samplers);

	const float windowWidth = graphics.GetWindowWidth();
	const float windowHeight = graphics.GetWindowHeight();
	bindables.push_back(std::make_unique<ScissorRectangle>());
	bindables.push_back(std::make_unique<Viewport>(windowWidth, windowHeight));
}

void Pass::Execute(Graphics& graphics)
{
	for (const auto& bindable : bindables)
	{
		bindable->Bind(graphics.GetMainCommandList());
	}
	for (const auto& sharedBindable : sharedBindables)
	{
		sharedBindable->Bind(graphics.GetMainCommandList());
	}
}

PassType Pass::GetType() const noexcept
{
	return type;
}

RootSignature* Pass::GetRootSignature() noexcept
{
	return rootSignature.get();
}
