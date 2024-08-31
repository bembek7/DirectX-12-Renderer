#include "Gui.h"
#include "imgui.h"
#include "backends/imgui_impl_win32.h"
#include "backends/imgui_impl_dx11.h"
#include <d3d11.h>
#include "Actor.h"
#include "SceneComponent.h"

Gui::Gui(const HWND& hWnd, ID3D11Device* const device, ID3D11DeviceContext* const context)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX11_Init(device, context);
}

Gui::~Gui()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void Gui::BeginFrame()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	ImGui::ShowDemoWindow();
}

void Gui::EndFrame()
{
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void Gui::RenderActorControlWindow(Actor* const actor)
{
	if (actor)
	{
		const std::string actorFullName = actor->GetActorFullName();
		if (ImGui::Begin(actorFullName.c_str()))
		{
			RenderComponentTree(actor->rootComponent.get(), actor);
			SceneComponent* const actorsSelected = actorsSelectedComponentsMap[actor];
			if (actorsSelected)
			{
				ImGui::Text("Location");
				ImGui::DragFloat3("##Location", (float*)&actorsSelected->relativeLocation, 0.1f, -100.0f, 100.0f);

				ImGui::Text("Rotation");
				ImGui::DragFloat3("##Rotation", (float*)&actorsSelected->relativeRotation, 1.f, -180.0f, 180.0f);

				ImGui::Text("Scale");
				ImGui::DragFloat3("##Scale", (float*)&actorsSelected->relativeScale, 0.1f, 0.01f, 10.0f);
			}
		}
		ImGui::End();
	}
}

void Gui::RenderComponentTree(SceneComponent* const component, Actor* const actor)
{
	if (component)
	{
		if (ImGui::TreeNodeEx(component->GetComponentFullName().c_str(), ImGuiTreeNodeFlags_DefaultOpen))
		{
			if (ImGui::IsItemClicked())
			{
				actorsSelectedComponentsMap[actor] = component;
			}

			for (auto& child : component->children)
			{
				RenderComponentTree(child.get(), actor);
			}

			ImGui::TreePop();
		}
	}
}