#pragma once
#include <wrl\client.h>
#include <d3d12.h>
#include <string>
#include <memory>
#include <unordered_map>

class ShadersPool
{
public:
	static ShadersPool& GetInstance();
	ShadersPool(ShadersPool const&) = delete;
	void operator=(ShadersPool const&) = delete;

	std::shared_ptr<Microsoft::WRL::ComPtr<ID3DBlob>> GetShaderBlob(const std::wstring& shaderFileName);
private:
	ShadersPool() = default;
private:
	std::unordered_map<std::wstring, std::shared_ptr<Microsoft::WRL::ComPtr<ID3DBlob>>> blobsMap;
};
