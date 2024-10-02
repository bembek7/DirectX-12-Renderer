#pragma once
#include "Window.h"
#include <memory>

class App
{
public:
	int Run();

private:
	void HandleInput();
	void InitializeScene();
private:
	Window window = Window(1444, 810);
};
