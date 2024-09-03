#pragma once
#include "Bindable.h"
#include <vector>
#include <DirectXMath.h>
#include <wrl/client.h>

class VertexBuffer : public Bindable
{
public:
	VertexBuffer(Graphics& graphics, const std::vector<float>& vertices, const unsigned int vertexSize);
	virtual void Bind(Graphics& graphics) noexcept override;
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	unsigned int vertexSize;
};