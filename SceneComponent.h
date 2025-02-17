#pragma once
#include <DirectXMath.h>
#include <memory>
#include <vector>
#include <string>

enum class PassType;
class Pass;
class Graphics;
struct aiNode;
struct aiScene;
class Light;

class SceneComponent
{
	friend class Gui;
public:
	virtual ~SceneComponent() = default;

	static std::unique_ptr<SceneComponent> CreateComponent(const std::string& componentName);
	static std::unique_ptr<SceneComponent> CreateComponent(Graphics& graphics, const aiNode* const node, const aiScene* const scene);
	static std::unique_ptr<SceneComponent> LoadComponent(Graphics& graphics, const std::string& fileName);

	template <typename T>
	static T* AttachComponents(std::unique_ptr<SceneComponent> child, SceneComponent* const newParent)
	{
		static_assert(std::is_base_of<SceneComponent, T>::value, "T must be derived from SceneComponent");
		T* const rawChild = reinterpret_cast<T*>(child.get());
		if (child->parent)
		{
			child->DeattachFromParent();
		}
		if (newParent)
		{
			child->parent = newParent;
			newParent->children.push_back(std::move(child));
		}
		return rawChild;
	}

	virtual void Draw(Graphics& graphics, const std::vector<Light*>& lights);
	virtual void Draw(Graphics& graphics, const PassType& passType);
	virtual void Update(Graphics& graphics);
	virtual void PrepareForPass(Graphics& graphics, Pass* const pass);

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
	DirectX::XMVECTOR GetComponentForwardVector() const noexcept;
	DirectX::XMVECTOR GetComponentUpVector() const noexcept;
	DirectX::XMVECTOR GetComponentRightVector() const noexcept;

	DirectX::XMVECTOR GetComponentRotationRadians() const noexcept;

	std::string GetComponentFullName();

protected:
	SceneComponent(const std::string& componentName);
	SceneComponent(Graphics& graphics, const aiNode* const node, const aiScene* const scene);

	virtual void RenderComponentDetails(Gui& gui);

private:
	void DeattachFromParent();
protected:
	std::vector<std::unique_ptr<SceneComponent>> children;
	SceneComponent* parent = nullptr;
	DirectX::XMFLOAT3 relativeLocation = { 0.f, 0.f, 0.f };
	DirectX::XMFLOAT3 relativeRotation = { 0.f, 0.f, 0.f };
	DirectX::XMFLOAT3 relativeScale = { 1.f, 1.f, 1.f };
	std::string componentName;
};
