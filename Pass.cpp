#include "Pass.h"
#include "Graphics.h"
#include "Camera.h"
#include "ScissorRectangle.h"
#include "Viewport.h"


Pass::Pass(Graphics& graphics, PassType type, const std::vector<RPD::CBTypes>& constantBuffers, const std::vector<RPD::TextureTypes>& textures, const std::vector<RPD::SamplerTypes>& samplers) :
	type(type)
{
	rootSignature = std::make_unique<RootSignature>(graphics, constantBuffers, textures, samplers);

	bindables.push_back(std::make_unique<ScissorRectangle>());

	pipelineStateStream.primitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	pipelineStateStream.rootSignature = rootSignature->Get();
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

void Pass::BindPassSpecific(ID3D12GraphicsCommandList* const drawingBundle)
{
}