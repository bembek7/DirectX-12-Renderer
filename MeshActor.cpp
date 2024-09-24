#include "MeshActor.h"
#include "MeshComponent.h"

MeshActor::MeshActor(Graphics& graphics, const std::string& fileName, const std::string& actorName) :
	Actor(actorName)
{
	SetRootComponent<MeshComponent>(SceneComponent::LoadComponent(graphics, fileName));
}