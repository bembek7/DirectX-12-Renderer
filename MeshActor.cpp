#include "MeshActor.h"

MeshActor::MeshActor(Graphics& graphics, const std::string& fileName, const ShaderType shaderType, const std::string& componentName, const std::string& actorName) :
	Actor(actorName)
{
	meshComp = SetRootComponent<MeshComponent>(std::move(MeshComponent::CreateComponent(graphics, fileName, shaderType, componentName)));
}