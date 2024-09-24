#pragma once
#include "Bindable.h"
#include <wrl\client.h>

class DepthStencilState : public Bindable
{
public:
	enum class Usage
	{
		Regular,
		Skybox
	};
	DepthStencilState(Graphics& graphics, const Usage usage);
	void Bind(Graphics& graphics) noexcept override;
private:
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;
};
