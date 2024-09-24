#pragma once
#include "Bindable.h"
#include <wrl\client.h>
#include "DepthStencilView.h"
#include <memory>
#include <vector>

class DepthCubeTexture : public Bindable
{
public:
	DepthCubeTexture(Graphics& graphics, const UINT slot, const UINT faceSize);
	virtual void Bind(Graphics& graphics) noexcept override;
	virtual void Clear(Graphics& graphics) noexcept;
	DepthStencilView* GetDepthBuffer(const unsigned int face);
	UINT GetFaceSize() const noexcept;

private:
	UINT slot = 0;
	UINT faceSize = 0;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureView;
	std::vector<std::unique_ptr<DepthStencilView>> depthBuffers;
};
