#include "Actor.h"

Actor::Actor(SceneComponent&& initialRootComponent)
{
	rootComponent = std::make_unique<SceneComponent>(std::move(initialRootComponent));
}

void Actor::AddActorScale(const DirectX::XMFLOAT3 scaleToAdd) noexcept
{
	rootComponent->AddRelativeScale(scaleToAdd);
}

void Actor::AddActorRotation(const DirectX::XMFLOAT3 rotationToAdd) noexcept
{
	rootComponent->AddRelativeRotation(rotationToAdd);
}

void Actor::AddActorLocation(const DirectX::XMFLOAT3 locationToAdd) noexcept
{
	rootComponent->AddRelativeLocation(locationToAdd);
}

void Actor::AddActorScale(const DirectX::XMVECTOR scaleToAdd) noexcept
{
	rootComponent->AddRelativeScale(scaleToAdd);
}

void Actor::AddActorRotation(const DirectX::XMVECTOR rotationToAdd) noexcept
{
	rootComponent->AddRelativeRotation(rotationToAdd);
}

void Actor::AddActorLocation(const DirectX::XMVECTOR locationToAdd) noexcept
{
	rootComponent->AddRelativeLocation(locationToAdd);
}

void Actor::SetActorScale(const DirectX::XMFLOAT3 newScale) noexcept
{
	rootComponent->SetRelativeScale(newScale);
}

void Actor::SetActorRotation(const DirectX::XMFLOAT3 newRotation) noexcept
{
	rootComponent->SetRelativeRotation(newRotation);
}

void Actor::SetActorLocation(const DirectX::XMFLOAT3 newLocation) noexcept
{
	rootComponent->SetRelativeLocation(newLocation);
}

void Actor::SetActorScale(const DirectX::XMVECTOR newScale) noexcept
{
	rootComponent->SetRelativeScale(newScale);
}

void Actor::SetActorRotation(const DirectX::XMVECTOR newRotation) noexcept
{
	rootComponent->SetRelativeRotation(newRotation);
}

void Actor::SetActorLocation(const DirectX::XMVECTOR newLocation) noexcept
{
	rootComponent->SetRelativeLocation(newLocation);
}

DirectX::XMFLOAT3 Actor::GetActorScale() const noexcept
{
	return rootComponent->GetRelativeScale();
}

DirectX::XMFLOAT3 Actor::GetActorRotation() const noexcept
{
	return rootComponent->GetRelativeRotation();
}

DirectX::XMFLOAT3 Actor::GetActorLocation() const noexcept
{
	return rootComponent->GetRelativeLocation();
}

DirectX::XMVECTOR Actor::GetActorScaleVector() const noexcept
{
	return rootComponent->GetRelativeScaleVector();
}

DirectX::XMVECTOR Actor::GetActorRotationVector() const noexcept
{
	return rootComponent->GetRelativeRotationVector();
}

DirectX::XMVECTOR Actor::GetActorLocationVector() const noexcept
{
	return rootComponent->GetRelativeLocationVector();
}
