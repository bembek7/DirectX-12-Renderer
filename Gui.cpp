#include "Gui.h"
#include "imgui.h"
#include "backends/imgui_impl_win32.h"
#include "backends/imgui_impl_dx12.h"
#include "Actor.h"
#include "SceneComponent.h"
#include "MeshComponent.h"
#include "PointLight.h"
#include <sstream>
#include "DirectionalLight.h"
#include "SpotLight.h"

Gui::Gui(const HWND& hWnd, ID3D12Device* const device, const UINT framesInFlightNum, const DXGI_FORMAT rtFormat)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	const D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc{
		.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
		.NumDescriptors = 1,
		.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
	};
	CHECK_HR(device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&srvHeap)));

	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX12_Init(device, framesInFlightNum, rtFormat, srvHeap.Get(), srvHeap->GetCPUDescriptorHandleForHeapStart(), srvHeap->GetGPUDescriptorHandleForHeapStart());
}

Gui::~Gui()
{
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void Gui::BeginFrame()
{
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void Gui::EndFrame(CD3DX12_CPU_DESCRIPTOR_HANDLE rtv, ID3D12GraphicsCommandList* const commandList)
{
	commandList->OMSetRenderTargets(1, &rtv, TRUE, nullptr);
	commandList->SetDescriptorHeaps(1, srvHeap.GetAddressOf());
	ImGui::Render();
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);
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

void Gui::RenderPerformanceInfo(const unsigned int fps, const float delayBetweenFrames)
{
	if (ImGui::Begin("Performance Info"))
	{
		std::stringstream ss;
		ss << "FPS:" << fps << " " << delayBetweenFrames << "ms";
		ImGui::Text(ss.str().c_str());
	}
	ImGui::End();
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
			selectedActor->RenderActorDetails(*this);
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
	if (component->GetMaterial()->roughnessBuffer)
	{
		ImGui::Text("Roughness");
		ImGui::DragFloat("##Roughness", (float*)&component->GetMaterial()->roughnessBuffer->roughness, 0.01f, 0.01f, 1.0f);
	}

	if (component->GetMaterial()->colorBuffer)
	{
		float maxWidth = 200.0f;
		float availableWidth = ImGui::GetContentRegionAvail().x;
		float itemWidth = std::clamp(availableWidth, 0.0f, maxWidth);
		ImGui::PushItemWidth(itemWidth);

		ImGui::ColorPicker3("##ColorPickerWidget", (float*)component->GetMaterial()->colorBuffer.get());

		ImGui::PopItemWidth();
	}
}

void Gui::RenderActorDetails(Actor* const actor)
{
	ImGui::Text("Actor Details");
	const std::string actorName = actor->GetActorFullName();
	ImGui::Text(actorName.c_str());
}

void Gui::RenderActorDetails(PointLight* const actor)
{
	ImGui::Text("Diffuse");
	ImGui::DragFloat3("##Diffuse", (float*)&actor->lightBuffer.diffuseColor, 0.01f, 0.0f, 1.0f);

	ImGui::Text("Ambient");
	ImGui::DragFloat3("##Ambient", (float*)&actor->lightBuffer.ambient, 0.01f, 0.0f, 1.0f);

	ImGui::Text("Diffuse Intensity");
	ImGui::DragFloat("##DiffuseIntensity", (float*)&actor->lightBuffer.diffuseIntensity, 0.01f, 0.0f, 1.0f);

	ImGui::Text("Specular Intensity");
	ImGui::DragFloat("##SpecularIntensity", (float*)&actor->lightBuffer.specularIntensity, 0.01f, 0.0f, 1.0f);

	ImGui::Text("Attenuation");
	ImGui::DragFloat("##Const", (float*)&actor->lightBuffer.attenuationConst, 0.0001f, 0.0f, 1.0f);
	ImGui::DragFloat("##Lin", (float*)&actor->lightBuffer.attenuationLin, 0.0001f, 0.0f, 1.0f);
	ImGui::DragFloat("##Quad", (float*)&actor->lightBuffer.attenuationQuad, 0.0001f, 0.0f, 1.0f);
}

void Gui::RenderActorDetails(DirectionalLight* const actor)
{
	ImGui::Text("Diffuse");
	ImGui::DragFloat3("##Diffuse", (float*)&actor->lightBuffer.diffuseColor, 0.01f, 0.0f, 1.0f);

	ImGui::Text("Ambient");
	ImGui::DragFloat3("##Ambient", (float*)&actor->lightBuffer.ambient, 0.01f, 0.0f, 1.0f);

	ImGui::Text("Diffuse Intensity");
	ImGui::DragFloat("##DiffuseIntensity", (float*)&actor->lightBuffer.diffuseIntensity, 0.01f, 0.0f, 1.0f);

	ImGui::Text("Specular Intensity");
	ImGui::DragFloat("##SpecularIntensity", (float*)&actor->lightBuffer.specularIntensity, 0.01f, 0.0f, 1.0f);
}

void Gui::RenderActorDetails(SpotLight* const actor)
{
	ImGui::Text("Diffuse");
	ImGui::DragFloat3("##Diffuse", (float*)&actor->lightBuffer.diffuseColor, 0.01f, 0.0f, 1.0f);

	ImGui::Text("Ambient");
	ImGui::DragFloat3("##Ambient", (float*)&actor->lightBuffer.ambient, 0.01f, 0.0f, 1.0f);

	ImGui::Text("Diffuse Intensity");
	ImGui::DragFloat("##DiffuseIntensity", (float*)&actor->lightBuffer.diffuseIntensity, 0.01f, 0.0f, 1.0f);

	ImGui::Text("Specular Intensity");
	ImGui::DragFloat("##SpecularIntensity", (float*)&actor->lightBuffer.specularIntensity, 0.01f, 0.0f, 1.0f);

	ImGui::Text("Spot Power");
	ImGui::DragFloat("##SpotPower", (float*)&actor->lightBuffer.spotPower, 0.01f, 0.0f, 1.0f);

	ImGui::Text("Attenuation");
	ImGui::DragFloat("##Const", (float*)&actor->lightBuffer.attenuationConst, 0.0001f, 0.0f, 1.0f);
	ImGui::DragFloat("##Lin", (float*)&actor->lightBuffer.attenuationLin, 0.0001f, 0.0f, 1.0f);
	ImGui::DragFloat("##Quad", (float*)&actor->lightBuffer.attenuationQuad, 0.0001f, 0.0f, 1.0f);
}