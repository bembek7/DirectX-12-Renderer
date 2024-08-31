#include "SceneComponent.h"
#include <imgui.h>
#include "Graphics.h"
#include "numbers"

SceneComponent::SceneComponent(const std::string& componentName) :
	componentName(componentName)
{}

void SceneComponent::DeattachFromParent()
{
	if (parent)
	{
		const auto it = std::find_if(parent->children.begin(), parent->children.end(), [this](auto& child) { return child.get() == this; });
		if (it != parent->children.end())
		{
			it->release();
		}

		parent = nullptr;
	}
}

std::unique_ptr<SceneComponent> SceneComponent::CreateComponent(const std::string& componentName)
{
	return std::unique_ptr<SceneComponent>(new SceneComponent(componentName));
}

void SceneComponent::Draw(Graphics& graphics)
{
	for (auto& child : children)
	{
		child->Draw(graphics);
	}
}

void SceneComponent::RenderShadowMap(Graphics& graphics)
{
	for (auto& child : children)
	{
		child->RenderShadowMap(graphics);
	}
}

DirectX::XMMATRIX SceneComponent::GetTransformMatrix() const noexcept
{
	return DirectX::XMMatrixScalingFromVector(GetComponentScaleVector()) *
		DirectX::XMMatrixRotationRollPitchYawFromVector(GetComponentRotationRadians()) *
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

DirectX::XMVECTOR SceneComponent::GetComponentForwardVector() const noexcept
{
	return DirectX::XMVector3Rotate({ 0.f, 0.f, 1.f }, DirectX::XMQuaternionRotationRollPitchYawFromVector(GetComponentRotationRadians()));
}

DirectX::XMVECTOR SceneComponent::GetComponentUpVector() const noexcept
{
	return DirectX::XMVector3Rotate({ 0.f, 1.f, 0.f }, DirectX::XMQuaternionRotationRollPitchYawFromVector(GetComponentRotationRadians()));
}

DirectX::XMVECTOR SceneComponent::GetComponentRightVector() const noexcept
{
	return DirectX::XMVector3Rotate({ 1.f, 0.f, 0.f }, DirectX::XMQuaternionRotationRollPitchYawFromVector(GetComponentRotationRadians()));
}

DirectX::XMVECTOR SceneComponent::GetComponentRotationRadians() const noexcept
{
	const DirectX::XMVECTOR rotationRadians = DirectX::XMVectorScale(GetComponentRotationVector(), float(std::numbers::pi) / 180.0f);
	return DirectX::XMVECTOR(rotationRadians);
}

std::string SceneComponent::GetComponentFullName()
{
	return componentName + " " + typeid(*this).name();
}