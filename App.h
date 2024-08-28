#pragma once
#include "Window.h"

class App
{
public:
	int Run();

private:
	Window window = Window(1280, 720);
};
