#pragma once
#include <wrl\client.h>
#include <d3d12.h>
#include <string>

class Graphics;

class TexLoader
{
public:
	static Microsoft::WRL::ComPtr<ID3D12Resource> LoadTextureFromFile(Graphics& graphics, const std::string& fileName);
};
