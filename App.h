#pragma once
#include "Window.h"
#include <memory>
#include "Scene.h"

class App
{
public:
	int Run();

private:
	enum class SceneType;
	void HandleInput();
	void InitializeScene(Graphics& graphics, SceneType sceneType, int lightSetup);
private:
	std::unique_ptr<Scene> scene;
	Window window = Window(1444, 810);
};
