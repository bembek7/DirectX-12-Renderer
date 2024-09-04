#pragma once
#include "BetterWindows.h"
#include <d3d11.h>
#include <unordered_map>

class Actor;
class SceneComponent;
class MeshComponent;

class Gui
{
public:
	Gui(const HWND& hWnd, ID3D11Device* const device, ID3D11DeviceContext* const context);
	~Gui();
	Gui(const Gui&) = delete;
	Gui& operator=(const Gui&) = delete;

	void BeginFrame();
	void EndFrame();

	void RenderActorTree(Actor* const actor);

	void RenderControlWindow();
	void RenderComponentDetails(SceneComponent* const component);
	void RenderComponentDetails(MeshComponent* const component);

private:
	void RenderComponentTree(SceneComponent* const component, Actor* const actor);

private:
	Actor* selectedActor = nullptr;
	SceneComponent* selectedComponent = nullptr;
};
