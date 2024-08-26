#pragma once
#include <DirectXMath.h>
#include "Actor.h"

class Camera : public Actor
{
public:
	DirectX::XMMATRIX GetMatrix() const noexcept;
};
