#pragma once
#include "MeshActor.h"
#include "ConstantBuffer.h"

class Graphics;
class Camera;

class Light : public MeshActor
{
public:
	void Bind(ID3D12GraphicsCommandList* const commandList);
	virtual void Update(Graphics& graphics) override;
	DirectX::XMMATRIX GetLightPerspective() const noexcept;

protected:
	Light(Graphics& graphics, const std::string& fileName, const std::string& actorName);
	virtual void RenderActorDetails(Gui& gui) override = 0;

protected:
	std::vector<std::unique_ptr<ConstantBuffer>> constantBuffers;

private:
	struct ShadowMapBuffer
	{
		DirectX::XMFLOAT4X4 lightPerspective;
	} shadowMapBuffer;

	Camera* shadowMapCamera;
};
