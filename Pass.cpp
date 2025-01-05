#include "Pass.h"
#include "Graphics.h"

void Pass::Execute(Graphics& graphics)
{
	graphics.SetProjection(projection);

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
