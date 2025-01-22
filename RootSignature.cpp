#include "RootSignature.h"
#include "d3dx12\d3dx12.h"
#include <stdexcept>
#include "ThrowMacros.h"
#include <DirectXMath.h>
#include "Graphics.h"

namespace Wrl = Microsoft::WRL;
namespace Dx = DirectX;

RootSignature::RootSignature(Graphics& graphics, const std::vector<RPD::CBTypes>& constantBuffers, const std::vector<RPD::TextureTypes>& textures, const std::vector<RPD::SamplerTypes>& samplers)
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

	UINT texIndex = 0;
	for (const auto& tex : textures)
	{
		texesDescRanges.push_back(D3D12_DESCRIPTOR_RANGE{ D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1u, RPD::texturesSlots.at(tex), 0u, texIndex });

		++texIndex;
	}
	rootParameters[index].InitAsDescriptorTable((UINT)texesDescRanges.size(), texesDescRanges.data(), D3D12_SHADER_VISIBILITY_PIXEL);

	const UINT samplersNum = (UINT)samplers.size();

	std::vector<CD3DX12_STATIC_SAMPLER_DESC> staticSamplers;
	staticSamplers.resize(samplersNum);

	size_t samplerIndex = 0;
	for (const auto& sampler : samplers)
	{
		const auto& samplerInfo = RPD::samplersInfo.at(sampler);

		staticSamplers[samplerIndex].Init(samplerInfo.slot, samplerInfo.filter);
		staticSamplers[samplerIndex].ShaderVisibility = samplerInfo.visibility;
		staticSamplers[samplerIndex].MaxAnisotropy = samplerInfo.maxAnisotropy;
		++samplerIndex;
	}

	const D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_MESH_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_AMPLIFICATION_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

	// TODO: deny access based on visibilities

	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.Init((UINT)rootParameters.size(), rootParameters.data(), samplersNum, staticSamplers.data(), rootSignatureFlags);

	// serialize root signature
	Wrl::ComPtr<ID3DBlob> signatureBlob;
	Wrl::ComPtr<ID3DBlob> errorBlob;
	if (const auto hr = D3D12SerializeRootSignature(
		&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		&signatureBlob, &errorBlob); FAILED(hr))
	{
		if (errorBlob)
		{
			auto errorBufferPtr = static_cast<const char*>(errorBlob->GetBufferPointer());
			throw std::runtime_error(errorBufferPtr);
		}
		CHECK_HR(hr);
	}
	// Create the root signature.
	CHECK_HR(graphics.GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature)));
}

ID3D12RootSignature* RootSignature::Get() noexcept
{
	return rootSignature.Get();
}

void RootSignature::Bind(ID3D12GraphicsCommandList* const commandList) noexcept
{
	commandList->SetGraphicsRootSignature(rootSignature.Get());
}