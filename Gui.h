#pragma once
#include "BetterWindows.h"
#include <unordered_map>
#include "d3dx12/d3dx12.h"

class Actor;
class PointLight;
class SceneComponent;
class MeshComponent;

class Gui
{
public:
	Gui(const HWND& hWnd, ID3D12Device* const device, const UINT framesInFlightNum, const DXGI_FORMAT rtFormat);
	~Gui();
	Gui(const Gui&) = delete;
	Gui& operator=(const Gui&) = delete;

	void BeginFrame();
	void EndFrame(ID3D12GraphicsCommandList* const commandList);

	void RenderActorTree(Actor* const actor);
	void RenderPerformanceInfo(const unsigned int fps, const float delayBetweenFrames);
	void RenderControlWindow();
	void RenderComponentDetails(SceneComponent* const component);
	void RenderComponentDetails(MeshComponent* const component);

	void RenderActorDetails(Actor* const actor);
	void RenderActorDetails(PointLight* const actor);
private:
	void RenderComponentTree(SceneComponent* const component, Actor* const actor);

private:
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvHeap;
	Actor* selectedActor = nullptr;
	SceneComponent* selectedComponent = nullptr;
};
