#pragma once
#include "Bindable.h"
#include <wrl\client.h>

class CubeTexture : public Bindable
{
public:
	CubeTexture(Graphics& graphics, const UINT slot, const std::string& path);
	virtual void Bind(Graphics& graphics) noexcept override;

	static std::string ResolveID(const UINT slot, const std::string& fileName) noexcept;

private:
	UINT slot;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureView;
};
