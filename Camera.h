#pragma once
#include <DirectXMath.h>
#include "SceneComponent.h"

class Camera : public SceneComponent
{
public:
	using SceneComponent::SceneComponent;

	DirectX::XMMATRIX GetMatrix() const noexcept;

	// will be moved lower in hierarchy later
	void AddMovementInput(const DirectX::XMFLOAT2 moveVector) noexcept; // +x going right -x going left -y going backward +y going forward, values between -1 and 1
	void AddPitchInput(const float pitchInput) noexcept;
	void AddYawInput(const float yawInput) noexcept;

private:
	const float movementSpeed = 0.3f;
	const float rotationSpeed = 0.001f;
};
