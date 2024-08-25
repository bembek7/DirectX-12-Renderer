#include "WorldObject.h"

WorldObject::WorldObject(const DirectX::XMVECTOR& location, const DirectX::XMVECTOR& scale, const DirectX::XMVECTOR& rotation) :
    location(location),
    scale(scale),
    rotation(rotation)
{}

DirectX::XMMATRIX WorldObject::GetTransformMatrix() const noexcept
{
    return DirectX::XMMatrixScalingFromVector(scale) * DirectX::XMMatrixRotationRollPitchYawFromVector(rotation) * DirectX::XMMatrixTranslationFromVector(location);
}

void WorldObject::AddScale(const DirectX::XMVECTOR& scaleToAdd) noexcept
{
    scale = DirectX::XMVectorAdd(scale, scaleToAdd);
}

void WorldObject::AddRotation(const DirectX::XMVECTOR& rotationToAdd) noexcept
{
    rotation = DirectX::XMVectorAdd(rotation, rotationToAdd);
}

void WorldObject::AddLocation(const DirectX::XMVECTOR& locationToAdd) noexcept
{
    location = DirectX::XMVectorAdd(location, locationToAdd);
}

void WorldObject::SetScale(const DirectX::XMVECTOR& newScale) noexcept
{
    scale = newScale;
}

void WorldObject::SetRotation(const DirectX::XMVECTOR& newRotation) noexcept
{
    rotation = newRotation;
}

void WorldObject::SetLocation(const DirectX::XMVECTOR& newLocation) noexcept
{
    location = newLocation;
}

DirectX::XMVECTOR WorldObject::GetScale() const noexcept
{
    return scale;
}

DirectX::XMVECTOR WorldObject::GetRotation() const noexcept
{
    return rotation;
}

DirectX::XMVECTOR WorldObject::GetLocation() const noexcept
{
    return location;
}
