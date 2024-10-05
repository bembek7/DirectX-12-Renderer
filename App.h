#pragma once
#include "Window.h"
#include <memory>
#include "Scene.h"

class App
{
public:
	int Run();

private:
	void HandleInput();
	void InitializeScene();
private:
	std::unique_ptr<Scene> scene;
	Window window = Window(1444, 810);
};
