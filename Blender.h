#pragma once
#include "Bindable.h"
#include <wrl\client.h>

class Blender : public Bindable
{
public:
	Blender(Graphics& graphics, bool blending);
	void Bind(Graphics& graphics) noexcept override;
private:
	Microsoft::WRL::ComPtr<ID3D11BlendState> blenderState;
	bool blending;
};
