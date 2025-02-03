#include "ShadersPool.h"
#include <d3dcompiler.h>
#include "ThrowMacros.h"

namespace Wrl = Microsoft::WRL;

ShadersPool& ShadersPool::GetInstance()
{
	static ShadersPool instance;
	return instance;
}

std::shared_ptr<Microsoft::WRL::ComPtr<ID3DBlob>> ShadersPool::GetShaderBlob(const std::wstring& shaderFileName)
{
	auto blobIt = blobsMap.find(shaderFileName);
	if (blobIt == blobsMap.end())
	{
		Wrl::ComPtr<ID3DBlob> shaderBlob;
		CHECK_HR(D3DReadFileToBlob(shaderFileName.c_str(), &shaderBlob));
		blobsMap[shaderFileName] = std::make_shared<Wrl::ComPtr<ID3DBlob>>(std::move(shaderBlob));
	}
	return blobsMap[shaderFileName];
}