#include "Camera.h"
#include "algorithm"

DirectX::XMMATRIX Camera::GetMatrix() const noexcept
{
	const auto camLocation = GetComponentLocationVector();
	const DirectX::XMVECTOR forwardVector = GetComponentForwardVector();
	const DirectX::XMVECTOR upVector = GetComponentUpVector();
	return DirectX::XMMatrixLookAtLH(camLocation, DirectX::XMVectorAdd(camLocation, forwardVector), upVector);
}

void Camera::AddMovementInput(const DirectX::XMFLOAT2 moveVector) noexcept
{
	const float XAxis = std::clamp(moveVector.x, -1.f, 1.f);
	const float YAxis = std::clamp(moveVector.y, -1.f, 1.f);
	AddRelativeLocation(DirectX::XMVectorScale(GetComponentRightVector(), XAxis));
	AddRelativeLocation(DirectX::XMVectorScale(GetComponentForwardVector(), YAxis));
}

void Camera::AddPitchInput(const float pitchInput) noexcept
{
	AddRelativeRotation(DirectX::XMVECTOR{ pitchInput, 0.f, 0.f });
}

void Camera::AddYawInput(const float yawInput) noexcept
{
	AddRelativeRotation(DirectX::XMVECTOR{ 0.f, yawInput, 0.f });
}