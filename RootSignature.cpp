#include "RootSignature.h"
#include "d3dx12\d3dx12.h"
#include <stdexcept>
#include "ThrowMacros.h"
#include <DirectXMath.h>

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

	// define static sampler
	CD3DX12_STATIC_SAMPLER_DESC staticSampler{ 1u, D3D12_FILTER_MIN_MAG_MIP_LINEAR };
	staticSampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootSignatureDesc.Init((UINT)std::size(rootParameters), rootParameters.data(), 1u, &staticSampler, rootSignatureFlags);
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
	CHECK_HR(GetDevice(graphics)->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature)));
}

ID3D12RootSignature* RootSignature::Get() noexcept
{
	return rootSignature.Get();
}

void RootSignature::Bind(ID3D12GraphicsCommandList* const commandList) noexcept
{
	commandList->SetGraphicsRootSignature(rootSignature.Get());
}