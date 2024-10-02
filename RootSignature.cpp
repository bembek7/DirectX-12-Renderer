#include "RootSignature.h"
#include "d3dx12\d3dx12.h"
#include <stdexcept>
#include "ThrowMacros.h"
#include <DirectXMath.h>

namespace Wrl = Microsoft::WRL;
namespace Dx = DirectX;

RootSignature::RootSignature(Graphics& graphics)
{
	// define root signature with a matrix of 16 32-bit floats used by the vertex shader (mvp matrix)
	CD3DX12_ROOT_PARAMETER rootParameters[1]{};
	rootParameters[0].InitAsConstants(sizeof(Dx::XMMATRIX) / 4, 0, 0, D3D12_SHADER_VISIBILITY_VERTEX);
	// Allow input layout and vertex shader and deny unnecessary access to certain pipeline stages.
	const D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_MESH_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_AMPLIFICATION_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;
	// define empty root signature
	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init((UINT)std::size(rootParameters), rootParameters, 0, nullptr, rootSignatureFlags);
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

void RootSignature::Bind(Graphics& graphics) noexcept
{
	GetCommandList(graphics)->SetGraphicsRootSignature(rootSignature.Get());
}