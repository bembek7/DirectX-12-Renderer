#include "MeshActor.h"

MeshActor::MeshActor(Graphics& graphics, const std::string& fileName, const std::string& actorName) :
	Actor(actorName)
{
	SetRootComponent<MeshComponent>(SceneComponent::LoadComponent(graphics, fileName));
}