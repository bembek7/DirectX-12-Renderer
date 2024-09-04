#include "SceneComponent.h"
#include <imgui.h>
#include "Graphics.h"
#include "numbers"
#include "BetterWindows.h"
#include <assimp\scene.h>
#include "MeshComponent.h"
#include <assimp\Importer.hpp>
#include <stdexcept>
#include <assimp\postprocess.h>
#include "Gui.h"

SceneComponent::SceneComponent(const std::string& componentName) :
	componentName(componentName)
{}

SceneComponent::SceneComponent(Graphics& graphics, const aiNode* const node, const aiScene* const scene)
{
	componentName = node->mName.C_Str();

	aiVector3D aiScale;
	aiVector3D aiRotation;
	aiVector3D aiLocation;
	node->mTransformation.Decompose(aiScale, aiRotation, aiLocation);
	aiRotation *= 180.0f / float(std::numbers::pi);
	DirectX::XMFLOAT3 scale = { aiScale.x, aiScale.y, aiScale.z };
	DirectX::XMFLOAT3 rotation = { aiRotation.x, aiRotation.y, aiRotation.z };
	DirectX::XMFLOAT3 location = { aiLocation.x, aiLocation.y, aiLocation.z };
	SetRelativeLocation(location);
	SetRelativeRotation(rotation);
	SetRelativeScale(scale);

	using SC = SceneComponent;
	for (size_t i = 0; i < node->mNumChildren; i++)
	{
		if (node->mChildren[i]->mNumMeshes > 0 || node->mChildren[i]->mNumChildren > 0)
		{
			std::unique_ptr<SceneComponent> childComponent;
			if (node->mChildren[i]->mNumMeshes == 0)
			{
				childComponent = std::move(SC::CreateComponent(graphics, node->mChildren[i], scene));
			}
			else
			{
				childComponent = std::move(MeshComponent::CreateComponent(graphics, node->mChildren[i], scene));
			}
			SC::AttachComponents<SC>(std::move(childComponent), this);
		}
	}
}

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

void SceneComponent::RenderComponentDetails(Gui& gui)
{
	gui.RenderComponentDetails(this);
}

std::unique_ptr<SceneComponent> SceneComponent::CreateComponent(const std::string& componentName)
{
	return std::unique_ptr<SceneComponent>(new SceneComponent(componentName));
}

std::unique_ptr<SceneComponent> SceneComponent::CreateComponent(Graphics& graphics, const aiNode* const node, const aiScene* const scene)
{
	return std::unique_ptr<SceneComponent>(new SceneComponent(graphics, node, scene));
}

std::unique_ptr<SceneComponent> SceneComponent::LoadComponent(Graphics& graphics, const std::string& fileName)
{
	Assimp::Importer importer;

	const aiScene* const scene = importer.ReadFile(fileName,
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType |
		aiProcess_GenNormals
	);

	if (!scene)
	{
		throw std::runtime_error(importer.GetErrorString());
	}

	if (scene->mRootNode)
	{
		std::unique_ptr<SceneComponent> newRootComponent;
		if (scene->mRootNode->mNumMeshes == 0)
		{
			newRootComponent = std::move(SceneComponent::CreateComponent(graphics, scene->mRootNode, scene));
		}
		else
		{
			newRootComponent = std::move(MeshComponent::CreateComponent(graphics, scene->mRootNode, scene));
		}
		return newRootComponent;
	}
	return std::unique_ptr<SceneComponent>();
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