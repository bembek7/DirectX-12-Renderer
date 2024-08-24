#pragma once
#include <DirectXMath.h>

class Camera
{
public:
	DirectX::XMMATRIX GetMatrix() const noexcept;
	void Move(const DirectX::XMVECTOR& moveVector) noexcept;
	void Rotate(const DirectX::XMVECTOR& rotateVector) noexcept;
	void SetPosition(const DirectX::XMVECTOR& newPosition) noexcept;
private:
	DirectX::XMVECTOR position = { 0.f, 0.f, 0.f };
	DirectX::XMVECTOR rotation = { 0.f, 0.f, 0.f };
};
