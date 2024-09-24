#pragma once
#include <memory>
#include "SceneComponent.h"
#include <string>

class Graphics;

class Actor
{
	friend class Gui;
public:
	Actor(const std::string& actorName = "Actor");

	virtual ~Actor() = default;

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
	void SetActorTransform(const DirectX::XMFLOAT3 newLocation, const DirectX::XMFLOAT3 newScale, const DirectX::XMFLOAT3 newRotation) noexcept;
	DirectX::XMFLOAT3 GetActorScale() const noexcept;
	DirectX::XMFLOAT3 GetActorRotation() const noexcept;
	DirectX::XMFLOAT3 GetActorLocation() const noexcept;
	DirectX::XMVECTOR GetActorScaleVector() const noexcept;
	DirectX::XMVECTOR GetActorRotationVector() const noexcept;
	DirectX::XMVECTOR GetActorLocationVector() const noexcept;

	void Draw(Graphics& graphics);
	void RenderShadowMap(Graphics& graphics);

	std::string GetActorFullName();

protected:
	// this should be used for setting the root
	template <typename T>
	T* SetRootComponent(std::unique_ptr<SceneComponent> newRoot)
	{
		if (!rootComponent)
		{
			static_assert(std::is_base_of<SceneComponent, T>::value, "T must be derived from SceneComponent");
			T* const rawComp = reinterpret_cast<T*>(newRoot.get());
			rootComponent = std::move(newRoot);
			return rawComp;
		}
		else
		{
			throw "Root is already set";
		}
	}

	virtual void RenderActorDetails(Gui& gui);

protected:
	std::string actorName;
	std::unique_ptr<SceneComponent> rootComponent;
};
