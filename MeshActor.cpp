#include "MeshActor.h"

MeshActor::MeshActor(const std::string& actorName) :
	Actor(actorName)
{
	meshComp = SetRootComponent<MeshComponent>(std::move(MeshComponent::CreateComponent("Mesh")));
}

void MeshActor::SetMesh(std::unique_ptr<Mesh> newMesh)
{
	meshComp->SetMesh(std::move(newMesh));
}