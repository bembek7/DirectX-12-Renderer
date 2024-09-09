#pragma once
#include "Bindable.h"
#include <wrl\client.h>

class Blender : public Bindable
{
public:
	Blender(Graphics& graphics, const bool blending);
	void Bind(Graphics& graphics) noexcept override;
	static std::string ResolveID(const bool blending) noexcept;
private:
	Microsoft::WRL::ComPtr<ID3D11BlendState> blenderState;
	bool blending;
};
