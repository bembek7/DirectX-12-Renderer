#pragma once
#include "MeshActor.h"
#include "ConstantBuffer.h"

class Graphics;
class Camera;

enum class LightType
{
	Point,
	Spot,
	Directional
};

class Light : public MeshActor
{
public:
	void Bind(ID3D12GraphicsCommandList* const commandList);
	virtual void Update(Graphics& graphics) override;
	const Camera* GetLightCamera() const noexcept;
	DirectX::XMFLOAT4X4 GetLightProjection() const noexcept;

	LightType GetType() const noexcept;

protected:
	Light(Graphics& graphics, const std::string& fileName, const std::string& actorName, const LightType type);
	virtual void RenderActorDetails(Gui& gui) override = 0;

protected:
	std::vector<std::unique_ptr<ConstantBuffer>> constantBuffers;
	DirectX::XMFLOAT4X4 projection{};
	Camera* shadowMapCamera;
	LightType type;
};
