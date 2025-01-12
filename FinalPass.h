#pragma once
#include "Pass.h"

class Graphics;
class Actor;


class FinalPass : public Pass
{
public:
	FinalPass(Graphics& graphics);

	void Execute(Graphics& graphics);
private:

};

