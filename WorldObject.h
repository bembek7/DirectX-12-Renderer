#pragma once
#include <DirectXMath.h>
class WorldObject
{
public:
	WorldObject() = default;
	WorldObject(const DirectX::XMVECTOR& location, const DirectX::XMVECTOR& scale = { 1.f, 1.f, 1.f }, const DirectX::XMVECTOR& rotation = { 0.f, 0.f, 0.f });

	DirectX::XMMATRIX GetTransformMatrix() const noexcept;
	void AddScale(const DirectX::XMVECTOR& scaleToAdd) noexcept;
	void AddRotation(const DirectX::XMVECTOR& rotationToAdd) noexcept;
	void AddLocation(const DirectX::XMVECTOR& locationToAdd) noexcept;
	void SetScale(const DirectX::XMVECTOR& newScale) noexcept;
	void SetRotation(const DirectX::XMVECTOR& newRotation) noexcept;
	void SetLocation(const DirectX::XMVECTOR& newLocation) noexcept;
	DirectX::XMVECTOR GetScale() const noexcept;
	DirectX::XMVECTOR GetRotation() const noexcept;
	DirectX::XMVECTOR GetLocation() const noexcept;
protected:
	DirectX::XMVECTOR location = { 0.f, 0.f, 0.f };
	DirectX::XMVECTOR rotation = { 0.f, 0.f, 0.f };
	DirectX::XMVECTOR scale = { 1.f, 1.f, 1.f };
};

