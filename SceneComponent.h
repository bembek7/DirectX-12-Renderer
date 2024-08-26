#pragma once
#include <DirectXMath.h>
#include <memory>
#include <vector>

class SceneComponent
{
public:
	// Parent should be nullptr only if object is root component
	SceneComponent(SceneComponent* const parent, DirectX::XMFLOAT3 relativeLocation = { 0.f, 0.f, 0.f }, DirectX::XMFLOAT3 relativeScale = { 1.f, 1.f, 1.f }, DirectX::XMFLOAT3 relativeRotation = { 0.f, 0.f, 0.f });

	DirectX::XMMATRIX GetTransformMatrix() const noexcept;
	void AddRelativeScale(const DirectX::XMFLOAT3 scaleToAdd) noexcept;
	void AddRelativeRotation(const DirectX::XMFLOAT3 rotationToAdd) noexcept;
	void AddRelativeLocation(const DirectX::XMFLOAT3 locationToAdd) noexcept;
	void AddRelativeScale(const DirectX::XMVECTOR scaleToAdd) noexcept;
	void AddRelativeRotation(const DirectX::XMVECTOR rotationToAdd) noexcept;
	void AddRelativeLocation(const DirectX::XMVECTOR locationToAdd) noexcept;
	void SetRelativeScale(const DirectX::XMFLOAT3 newScale) noexcept;
	void SetRelativeRotation(const DirectX::XMFLOAT3 newRotation) noexcept;
	void SetRelativeLocation(const DirectX::XMFLOAT3 newLocation) noexcept;
	void SetRelativeScale(const DirectX::XMVECTOR newScale) noexcept;
	void SetRelativeRotation(const DirectX::XMVECTOR newRotation) noexcept;
	void SetRelativeLocation(const DirectX::XMVECTOR newLocation) noexcept;
	DirectX::XMFLOAT3 GetRelativeScale() const noexcept;
	DirectX::XMFLOAT3 GetRelativeRotation() const noexcept;
	DirectX::XMFLOAT3 GetRelativeLocation() const noexcept;
	DirectX::XMVECTOR GetRelativeScaleVector() const noexcept;
	DirectX::XMVECTOR GetRelativeRotationVector() const noexcept;
	DirectX::XMVECTOR GetRelativeLocationVector() const noexcept;
	DirectX::XMFLOAT3 GetComponentScale() const noexcept;
	DirectX::XMFLOAT3 GetComponentRotation() const noexcept;
	DirectX::XMFLOAT3 GetComponentLocation() const noexcept;
	DirectX::XMVECTOR GetComponentScaleVector() const noexcept;
	DirectX::XMVECTOR GetComponentRotationVector() const noexcept;
	DirectX::XMVECTOR GetComponentLocationVector() const noexcept;
protected:
	//std::vector<std::unique_ptr<SceneComponent>> children;
	SceneComponent* parent = nullptr;
	DirectX::XMFLOAT3 relativeLocation = { 0.f, 0.f, 0.f };
	DirectX::XMFLOAT3 relativeRotation = { 0.f, 0.f, 0.f };
	DirectX::XMFLOAT3 relativeScale = { 1.f, 1.f, 1.f };
};

