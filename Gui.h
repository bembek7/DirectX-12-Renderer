#pragma once
#include "BetterWindows.h"
#include <d3d12.h>
#include <unordered_map>

class Actor;
class PointLight;
class SceneComponent;
class MeshComponent;

class Gui
{
public:
	Gui(const HWND& hWnd, ID3D12Device* const device, const UINT framesInFlightNum, const DXGI_FORMAT rtFormat, ID3D12DescriptorHeap* const srvDescHeap,
		const D3D12_CPU_DESCRIPTOR_HANDLE& cpuDescHandle, const D3D12_GPU_DESCRIPTOR_HANDLE& gpuDescHandle);
	~Gui();
	Gui(const Gui&) = delete;
	Gui& operator=(const Gui&) = delete;

	void BeginFrame();
	void EndFrame(ID3D12GraphicsCommandList* const commandList);

	void RenderActorTree(Actor* const actor);

	void RenderControlWindow();
	void RenderComponentDetails(SceneComponent* const component);
	void RenderComponentDetails(MeshComponent* const component);

	void RenderActorDetails(Actor* const actor);
	void RenderActorDetails(PointLight* const actor);
private:
	void RenderComponentTree(SceneComponent* const component, Actor* const actor);

private:
	Actor* selectedActor = nullptr;
	SceneComponent* selectedComponent = nullptr;
};
