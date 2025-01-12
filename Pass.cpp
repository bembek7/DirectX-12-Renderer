#include "Pass.h"
#include "Graphics.h"
#include "Camera.h"

Pass::Pass(const Camera* const camera, DirectX::XMFLOAT4X4 projection, PassType type, 
	const std::vector<RPD::CBTypes>& constantBuffers, const std::vector<RPD::TextureTypes>& textures) noexcept :
	cameraUsed(camera), projection(projection), type(type), constantBuffers(constantBuffers), textures(textures)
{
}

void Pass::Execute(Graphics& graphics, const std::vector<std::unique_ptr<Actor>>& actors)
{
	graphics.SetProjection(projection);
	graphics.SetCamera(cameraUsed->GetMatrix());

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

std::vector<CD3DX12_ROOT_PARAMETER> Pass::InitRootParameters() noexcept
{
	std::vector<CD3DX12_ROOT_PARAMETER> rootParameters;
	auto rpSize = constantBuffers.size();
	if (textures.size() > 0)
	{
		++rpSize;
	}
	rootParameters.resize(rpSize);

	size_t index = 0;
	for (const auto& cb : constantBuffers)
	{
		const auto& cbInfo = RPD::cbsInfo.at(cb);
		if (cbInfo.size > 0) // assuming every cb that is supposed to be constant has size provided
		{
			rootParameters[index].InitAsConstants(cbInfo.size / 4, cbInfo.slot, 0u, cbInfo.visibility);
		}
		else
		{
			rootParameters[index].InitAsConstantBufferView(cbInfo.slot, 0u, cbInfo.visibility);
		}

		++index;
	}

	D3D12_DESCRIPTOR_RANGE{};
	UINT texIndex = 0;
	for (const auto& tex : textures)
	{
		texesDescRanges.push_back(D3D12_DESCRIPTOR_RANGE{ D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1u, RPD::texturesSlots.at(tex), 0u, texIndex });

		++texIndex;
	}
	rootParameters[index].InitAsDescriptorTable((UINT)texesDescRanges.size(), texesDescRanges.data(), D3D12_SHADER_VISIBILITY_PIXEL);

	return rootParameters;
}