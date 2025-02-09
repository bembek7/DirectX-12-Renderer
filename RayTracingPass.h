#pragma once
#include "Pass.h"


class Graphics;
class Camera;
class Actor;

class RayTracingPass : Pass
{
public:
	RayTracingPass(Graphics& graphics);

	void Execute(Graphics& graphics);
};

