#include "SceneComponent.h"

SceneComponent::SceneComponent(SceneComponent* const parent, const DirectX::XMFLOAT3 relativeLocation, const DirectX::XMFLOAT3 relativeScale, const DirectX::XMFLOAT3 relativeRotation) :
    parent(parent),
    relativeLocation(relativeLocation),
    relativeScale(relativeScale),
    relativeRotation(relativeRotation)
{}

DirectX::XMMATRIX SceneComponent::GetTransformMatrix() const noexcept
{
    return DirectX::XMMatrixScalingFromVector(GetComponentScaleVector()) *
        DirectX::XMMatrixRotationRollPitchYawFromVector(GetComponentRotationVector()) *
        DirectX::XMMatrixTranslationFromVector(GetComponentLocationVector());
}

void SceneComponent::AddRelativeScale(const DirectX::XMFLOAT3 scaleToAdd) noexcept
{
    AddRelativeScale(DirectX::XMLoadFloat3(&scaleToAdd));
}

void SceneComponent::AddRelativeRotation(const DirectX::XMFLOAT3 rotationToAdd) noexcept
{
    AddRelativeRotation(DirectX::XMLoadFloat3(&rotationToAdd));
}

void SceneComponent::AddRelativeLocation(const DirectX::XMFLOAT3 locationToAdd) noexcept
{
    AddRelativeLocation(DirectX::XMLoadFloat3(&locationToAdd));
}

void SceneComponent::AddRelativeScale(const DirectX::XMVECTOR scaleToAdd) noexcept
{
    DirectX::XMStoreFloat3(&relativeScale, DirectX::XMVectorAdd(GetRelativeScaleVector(), scaleToAdd));
}

void SceneComponent::AddRelativeRotation(const DirectX::XMVECTOR rotationToAdd) noexcept
{
    DirectX::XMStoreFloat3(&relativeRotation, DirectX::XMVectorAdd(GetRelativeRotationVector(), rotationToAdd));
}

void SceneComponent::AddRelativeLocation(const DirectX::XMVECTOR locationToAdd) noexcept
{
    DirectX::XMStoreFloat3(&relativeLocation, DirectX::XMVectorAdd(GetRelativeLocationVector(), locationToAdd));
}

void SceneComponent::SetRelativeScale(const DirectX::XMFLOAT3 newScale) noexcept
{
    relativeScale = newScale;
}

void SceneComponent::SetRelativeRotation(const DirectX::XMFLOAT3 newRotation) noexcept
{
    relativeRotation = newRotation;
}

void SceneComponent::SetRelativeLocation(const DirectX::XMFLOAT3 newLocation) noexcept
{
    relativeLocation = newLocation;
}

void SceneComponent::SetRelativeScale(const DirectX::XMVECTOR newScale) noexcept
{
    DirectX::XMStoreFloat3(&relativeScale, newScale);
}

void SceneComponent::SetRelativeRotation(const DirectX::XMVECTOR newRotation) noexcept
{
    DirectX::XMStoreFloat3(&relativeRotation, newRotation);
}

void SceneComponent::SetRelativeLocation(const DirectX::XMVECTOR newLocation) noexcept
{
    DirectX::XMStoreFloat3(&relativeLocation, newLocation);
}

DirectX::XMFLOAT3 SceneComponent::GetRelativeScale() const noexcept
{
    return relativeScale;
}

DirectX::XMFLOAT3 SceneComponent::GetRelativeRotation() const noexcept
{
    return relativeRotation;
}

DirectX::XMFLOAT3 SceneComponent::GetRelativeLocation() const noexcept
{
    return relativeLocation;
}

DirectX::XMVECTOR SceneComponent::GetRelativeScaleVector() const noexcept
{
    return DirectX::XMLoadFloat3(&relativeScale);
}

DirectX::XMVECTOR SceneComponent::GetRelativeRotationVector() const noexcept
{
    return DirectX::XMLoadFloat3(&relativeRotation);
}

DirectX::XMVECTOR SceneComponent::GetRelativeLocationVector() const noexcept
{
    return DirectX::XMLoadFloat3(&relativeLocation);
}

DirectX::XMFLOAT3 SceneComponent::GetComponentScale() const noexcept
{
    DirectX::XMFLOAT3 compRotation;
    DirectX::XMStoreFloat3(&compRotation, GetComponentScaleVector());
    return compRotation;
}

DirectX::XMFLOAT3 SceneComponent::GetComponentRotation() const noexcept
{
    DirectX::XMFLOAT3 compRotation;
    DirectX::XMStoreFloat3(&compRotation, GetComponentRotationVector());
    return compRotation;
}

DirectX::XMFLOAT3 SceneComponent::GetComponentLocation() const noexcept
{
    DirectX::XMFLOAT3 compLocation;
    DirectX::XMStoreFloat3(&compLocation, GetComponentLocationVector());
    return compLocation;
}

DirectX::XMVECTOR SceneComponent::GetComponentScaleVector() const noexcept
{
    if (parent)
    {
        return DirectX::XMVectorMultiply(parent->GetComponentScaleVector(), GetRelativeScaleVector());
    }
    return GetRelativeScaleVector();
}

DirectX::XMVECTOR SceneComponent::GetComponentRotationVector() const noexcept
{
    if (parent)
    {
        return DirectX::XMVectorAdd(parent->GetComponentRotationVector(), GetRelativeRotationVector());
    }
    return GetRelativeRotationVector();
}

DirectX::XMVECTOR SceneComponent::GetComponentLocationVector() const noexcept
{
    if (parent)
    {
        return DirectX::XMVectorAdd(parent->GetComponentLocationVector(), GetRelativeLocationVector());
    }
    return GetRelativeLocationVector();
}
