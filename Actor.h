#pragma once
#include <memory>
#include "SceneComponent.h"

class Actor
{
public:
	Actor() = default;
	Actor(SceneComponent&& rootComponent);

	void AddActorScale(const DirectX::XMFLOAT3 scaleToAdd) noexcept;
	void AddActorRotation(const DirectX::XMFLOAT3 rotationToAdd) noexcept;
	void AddActorLocation(const DirectX::XMFLOAT3 locationToAdd) noexcept;
	void AddActorScale(const DirectX::XMVECTOR scaleToAdd) noexcept;
	void AddActorRotation(const DirectX::XMVECTOR rotationToAdd) noexcept;
	void AddActorLocation(const DirectX::XMVECTOR locationToAdd) noexcept;
	void SetActorScale(const DirectX::XMFLOAT3 newScale) noexcept;
	void SetActorRotation(const DirectX::XMFLOAT3 newRotation) noexcept;
	void SetActorLocation(const DirectX::XMFLOAT3 newLocation) noexcept;
	void SetActorScale(const DirectX::XMVECTOR newScale) noexcept;
	void SetActorRotation(const DirectX::XMVECTOR newRotation) noexcept;
	void SetActorLocation(const DirectX::XMVECTOR newLocation) noexcept;
	DirectX::XMFLOAT3 GetActorScale() const noexcept;
	DirectX::XMFLOAT3 GetActorRotation() const noexcept;
	DirectX::XMFLOAT3 GetActorLocation() const noexcept;
	DirectX::XMVECTOR GetActorScaleVector() const noexcept;
	DirectX::XMVECTOR GetActorRotationVector() const noexcept;
	DirectX::XMVECTOR GetActorLocationVector() const noexcept;

protected:
	std::unique_ptr<SceneComponent> rootComponent;
};

