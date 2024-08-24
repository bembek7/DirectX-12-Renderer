#include "Camera.h"

DirectX::XMMATRIX Camera::GetMatrix() const noexcept
{
	const DirectX::XMVECTOR forwardVector = DirectX::XMVector3Rotate({ 0.f, 0.f, 1.f }, DirectX::XMQuaternionRotationRollPitchYawFromVector(rotation));
	const DirectX::XMVECTOR upVector = DirectX::XMVector3Rotate({ 0.f, 1.f, 0.f, 0.f }, DirectX::XMQuaternionRotationRollPitchYawFromVector(rotation));
	return DirectX::XMMatrixLookAtLH(position, DirectX::XMVectorAdd(position, forwardVector), upVector);
}

void Camera::Move(const DirectX::XMVECTOR& moveVector) noexcept
{
	const DirectX::XMVECTOR moveVectorRotated = DirectX::XMVector3Rotate(moveVector, DirectX::XMQuaternionRotationRollPitchYawFromVector(rotation));
	position = DirectX::XMVectorAdd(position, moveVectorRotated);
}

void Camera::Rotate(const DirectX::XMVECTOR& rotateVector) noexcept
{
	rotation = DirectX::XMVectorAdd(rotation, rotateVector);
}

void Camera::SetPosition(const DirectX::XMVECTOR& newPosition) noexcept
{
	position = newPosition;
}
