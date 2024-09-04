#include "Gui.h"
#include "imgui.h"
#include "backends/imgui_impl_win32.h"
#include "backends/imgui_impl_dx11.h"
#include <d3d11.h>
#include "Actor.h"
#include "SceneComponent.h"
#include "MeshComponent.h"
#include <sstream>

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

void Gui::RenderActorTree(Actor* const actor)
{
	if (actor)
	{
		const std::string actorFullName = actor->GetActorFullName();
		if (ImGui::Begin(actorFullName.c_str()))
		{
			RenderComponentTree(actor->rootComponent.get(), actor);
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
				selectedActor = actor;
				selectedComponent = component;
			}

			for (auto& child : component->children)
			{
				RenderComponentTree(child.get(), actor);
			}

			ImGui::TreePop();
		}
	}
}

void Gui::RenderControlWindow()
{
	if (selectedActor && selectedComponent)
	{
		const std::string componentName = selectedComponent->GetComponentFullName();
		const std::string actorName = selectedActor->GetActorFullName();
		std::stringstream ss;
		ss << componentName << " | " << actorName;
		if (ImGui::Begin("Control window"))
		{
			ImGui::Text(ss.str().c_str());

			selectedComponent->RenderComponentDetails(*this);
		}
		ImGui::End();
	}
}

void Gui::RenderComponentDetails(SceneComponent* const component)
{
	ImGui::Text("Relative Location");
	ImGui::DragFloat3("##Location", (float*)&component->relativeLocation, 0.1f, -100.0f, 100.0f);

	ImGui::Text("Relative Rotation");
	ImGui::DragFloat3("##Rotation", (float*)&component->relativeRotation, 1.f, -180.0f, 180.0f);

	ImGui::Text("Relative Scale");
	ImGui::DragFloat3("##Scale", (float*)&component->relativeScale, 0.1f, 0.01f, 10.0f);
}

void Gui::RenderComponentDetails(MeshComponent* const component)
{
	RenderComponentDetails(reinterpret_cast<SceneComponent*>(component));
	ImGui::Text("Roughness");

	ImGui::DragFloat("##Roughness", (float*)&component->GetMaterial()->roughnessBuffer.roughness, 0.01f, 0.01f, 1.0f);

	if (component->GetMaterial()->colorBuffer)
	{
		float maxWidth = 200.0f;
		float availableWidth = ImGui::GetContentRegionAvail().x;
		float itemWidth = std::clamp(availableWidth, 0.0f, maxWidth);
		ImGui::PushItemWidth(itemWidth);

		ImGui::ColorPicker4("##ColorPickerWidget", (float*)component->GetMaterial()->colorBuffer.get());

		ImGui::PopItemWidth();
	}
}