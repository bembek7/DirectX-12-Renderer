#pragma once
#include "Actor.h"
#include "MeshComponent.h"

class MeshActor : public Actor
{
public:
	MeshActor(Graphics& graphics, const std::string& fileName, const std::string& actorName = "Mesh Actor");
protected:
	MeshComponent* meshComp;
};
