#include "Pass.h"
#include "Graphics.h"

void Pass::Execute(Graphics& graphics)
{
	graphics.SetProjection(projection);

	for (const auto& bindable : bindables)
	{
		bindable->Update(graphics);
		bindable->Bind(graphics);
	}
	for (const auto& sharedBindable : sharedBindables)
	{
		sharedBindable->Update(graphics);
		sharedBindable->Bind(graphics);
	}
}