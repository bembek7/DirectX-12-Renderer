#include "RootSignature.h"
#include "d3dx12\d3dx12.h"
#include <stdexcept>
#include "ThrowMacros.h"

namespace Wrl = Microsoft::WRL;

RootSignature::RootSignature(Graphics& graphics)
{
	// define empty root signature
	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
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