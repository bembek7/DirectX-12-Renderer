#include "Pass.h"
#include "Graphics.h"
#include "Camera.h"

Pass::Pass(const Camera* camera, DirectX::XMFLOAT4X4 projection) noexcept :
	cameraUsed(camera), projection(projection)
{
}

void Pass::Execute(Graphics& graphics, const std::vector<std::unique_ptr<Actor>>& actors)
{
	graphics.SetProjection(projection);
	graphics.SetCamera(cameraUsed->GetMatrix());

	for (const auto& bindable : bindables)
	{
		bindable->Bind(graphics.GetMainCommandList());
	}
	for (const auto& sharedBindable : sharedBindables)
	{
		sharedBindable->Bind(graphics.GetMainCommandList());
	}
}

PassType Pass::GetType() const noexcept
{
	return type;
}

RootSignature* Pass::GetRootSignature() noexcept
{
	return rootSignature.get();
}
