#pragma once
#include "Actor.h"

class MeshComponent;

class MeshActor : public Actor
{
public:
	MeshActor(Graphics& graphics, const std::string& fileName, const std::string& actorName = "Mesh Actor");
protected:
	MeshComponent* meshComp;
};
