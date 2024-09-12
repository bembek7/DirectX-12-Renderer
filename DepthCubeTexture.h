#pragma once
#include "Bindable.h"
#include <wrl\client.h>
#include "DepthStencilView.h"
#include <memory>
#include <vector>

class DepthCubeTexture : public Bindable
{
public:
	DepthCubeTexture(Graphics& graphics, const UINT slot, const UINT size);
	virtual void Bind(Graphics& graphics) noexcept override;

	DepthStencilView* GetDepthBuffer(const unsigned int face);
private:
	UINT slot = 0;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureView;
	std::vector<std::unique_ptr<DepthStencilView>> depthBuffers;
};
