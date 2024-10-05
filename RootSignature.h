#pragma once
#include "Bindable.h"
#include <wrl/client.h>

class RootSignature : public Bindable
{
public:
	RootSignature(Graphics& graphics, const std::vector<CD3DX12_ROOT_PARAMETER>& rootParameters);
	ID3D12RootSignature* Get() noexcept;
	virtual void Bind(Graphics& graphics) noexcept override;
private:
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
};
