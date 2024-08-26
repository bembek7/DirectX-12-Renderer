#include "Camera.h"

DirectX::XMMATRIX Camera::GetMatrix() const noexcept
{
	const auto camRotation = GetActorRotationVector();
	const auto camLocation = GetActorLocationVector();
	const DirectX::XMVECTOR forwardVector = DirectX::XMVector3Rotate({ 0.f, 0.f, 1.f }, DirectX::XMQuaternionRotationRollPitchYawFromVector(camRotation));
	const DirectX::XMVECTOR upVector = DirectX::XMVector3Rotate({ 0.f, 1.f, 0.f, 0.f }, DirectX::XMQuaternionRotationRollPitchYawFromVector(camRotation));
	return DirectX::XMMatrixLookAtLH(camLocation, DirectX::XMVectorAdd(camLocation, forwardVector), upVector);
}
