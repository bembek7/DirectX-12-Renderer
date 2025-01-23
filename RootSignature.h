#pragma once
#include "Bindable.h"
#include "RootParametersDescription.h"
#include <wrl/client.h>

class Graphics;

class RootSignature : public Bindable
{
public:
	RootSignature(Graphics& graphics, const std::vector<RPD::CBTypes>& constantBuffers, const std::vector<RPD::TextureTypes>& textures, const std::vector<RPD::SamplerTypes>& samplers);
	ID3D12RootSignature* Get() noexcept;
	virtual void Bind(ID3D12GraphicsCommandList* const commandList) noexcept override;
	UINT GetDescriptorTableIndex() const noexcept;
private:
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
	std::vector<D3D12_DESCRIPTOR_RANGE> texesDescRanges;
	UINT descriptorTableIndex = 0u;
};
