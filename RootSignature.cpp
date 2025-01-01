#include "RootSignature.h"
#include "d3dx12\d3dx12.h"
#include <stdexcept>
#include "ThrowMacros.h"
#include <DirectXMath.h>
#include "Graphics.h"

namespace Wrl = Microsoft::WRL;
namespace Dx = DirectX;

RootSignature::RootSignature(Graphics& graphics, const std::vector<CD3DX12_ROOT_PARAMETER>& rootParameters)
{
	// Allow input layout and vertex shader and deny unnecessary access to certain pipeline stages.
	const D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_MESH_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_AMPLIFICATION_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
	// define empty root signature
	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc{};

	// Define static samplers
	CD3DX12_STATIC_SAMPLER_DESC staticSamplers[2];

	// Comparison sampler
	staticSamplers[0].Init(0, D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR);
	staticSamplers[0].ShaderRegister = 0;
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	staticSamplers[0].BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
	staticSamplers[0].Filter = D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	// Anisotropic sampler
	staticSamplers[1].Init(1, D3D12_FILTER_ANISOTROPIC);
	staticSamplers[1].ShaderRegister = 1;
	staticSamplers[1].Filter = D3D12_FILTER_ANISOTROPIC;
	staticSamplers[1].MaxAnisotropy = D3D12_REQ_MAXANISOTROPY;
	staticSamplers[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	rootSignatureDesc.Init((UINT)rootParameters.size(), rootParameters.data(), 2, staticSamplers, rootSignatureFlags);

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