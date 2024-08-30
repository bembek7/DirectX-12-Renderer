#pragma once
#include "Actor.h"
#include "Mesh.h"
#include "MeshComponent.h"

class MeshActor : public Actor
{
public:
	MeshActor(const std::string& actorName = "MeshActor");
	void SetMesh(std::unique_ptr<Mesh> newMesh);
protected:
	MeshComponent* meshComp;
};
