#include "Camera.h"
#include "algorithm"

std::unique_ptr<Camera> Camera::CreateComponent(const std::string& componentName)
{
	return std::unique_ptr<Camera>(new Camera(componentName));
}

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
	AddRelativeLocation(DirectX::XMVectorScale(GetComponentRightVector(), XAxis * movementSpeed));
	AddRelativeLocation(DirectX::XMVectorScale(GetComponentForwardVector(), YAxis * movementSpeed));
}

void Camera::AddPitchInput(const float pitchInput) noexcept
{
	AddRelativeRotation(DirectX::XMVectorScale(DirectX::XMVECTOR{ pitchInput, 0.f, 0.f }, rotationSpeed));
}

void Camera::AddYawInput(const float yawInput) noexcept
{
	AddRelativeRotation(DirectX::XMVectorScale(DirectX::XMVECTOR{ 0.f, yawInput, 0.f }, rotationSpeed));
}