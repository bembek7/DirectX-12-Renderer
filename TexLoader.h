#pragma once
#include <wrl\client.h>
#include <d3d12.h>
#include <string>
#include <memory>
#include <unordered_map>

class Graphics;

class TexLoader
{
public:
	struct Image
	{
		Microsoft::WRL::ComPtr<ID3D12Resource> resource;
		bool hasAlpha = false;
	};
public:
	static TexLoader& GetInstance();
	TexLoader(TexLoader const&) = delete;
	void operator=(TexLoader const&) = delete;

	std::shared_ptr<Image> GetTexture(Graphics& graphics, const std::string& fileName);
private:
	static Image LoadTextureFromFile(Graphics& graphics, const std::string& fileName);
	TexLoader() = default;
private:
	std::unordered_map<std::string, std::weak_ptr<Image>> texturesMap;
};
