#include "PanelGameObject.h"

#include "Component/ComponentCamera.h"
#include "Component/ComponentCanvas.h"
#include "Component/ComponentMeshRenderer.h"
#include "Component/ComponentLight.h"
#include "Component/ComponentScript.h"
#include "Component/ComponentText.h"
#include "Component/ComponentTransform.h"
#include "Component/ComponentUI.h"
#include "Component/ComponentButton.h"

#include "EditorUI/Panel/PanelInspector.h"
#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleEditor.h"
#include "Module/ModuleScene.h"
#include "ResourceManagement/Resources/Prefab.h"

#include <imgui.h>
#include <imgui_stdlib.h>
#include <FontAwesome5/IconsFontAwesome5.h>

PanelGameObject::PanelGameObject()
{
	enabled = true;
	opened = true;
	window_name = "GameObject Inspector";
}

void PanelGameObject::Render(GameObject* game_object)
{
	if (game_object == nullptr)
	{
		return;
	}
	ImGui::Checkbox("", &game_object->active);

	ImGui::SameLine();
	ImGui::Text(ICON_FA_CUBE);

	ImGui::SameLine();
	ImGui::InputText("###GameObject name Input", &game_object->name);

	ImGui::SameLine();
	if (ImGui::Checkbox("Static", &game_object->is_static))
	{
		game_object->SetStatic(game_object->is_static);
	}

	ImGui::Spacing();
	if (game_object->prefab_reference != nullptr)
	{
		ShowPrefabMenu(game_object);
	}
	ImGui::Separator();
	ImGui::Spacing();

	component_panel.ShowComponentTransformWindow(&game_object->transform);

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	for (unsigned int i = 0; i < game_object->components.size(); ++i)
	{
		if (i != 0)
		{
			ImGui::Spacing();
			ImGui::Separator();
		}
		ImGui::Spacing();
		ImGui::PushID(i);

		Component* component = game_object->components[i];
		switch (component->GetType())
		{
			case Component::ComponentType::CAMERA:
				component_panel.ShowComponentCameraWindow(static_cast<ComponentCamera*>(component));
				break;
			case Component::ComponentType::MESH_RENDERER:
				component_panel.ShowComponentMeshRendererWindow(static_cast<ComponentMeshRenderer*>(component));
				break;
			case Component::ComponentType::LIGHT:
				component_panel.ShowComponentLightWindow(static_cast<ComponentLight*>(component));
				break;
			case Component::ComponentType::SCRIPT:
				component_panel.ShowComponentScriptWindow(static_cast<ComponentScript*>(component));
				break;
			case Component::ComponentType::UI:
				component_panel.ShowComponentUIWindow(static_cast<ComponentUI*>(component));
				break;
			default:
				break;
		}

		ImGui::PopID();
	}

	if (game_object->GetComponent(Component::ComponentType::MESH_RENDERER) != nullptr)
	{
		App->editor->inspector->material_panel.Render(static_cast<ComponentMeshRenderer*>(game_object->GetComponent(Component::ComponentType::MESH_RENDERER))->material_to_render.get());
	}

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	component_panel.ShowAddNewComponentButton();
}

void PanelGameObject::ShowPrefabMenu(GameObject* game_object)
{
	ImGui::SameLine();
	if(game_object->prefab_reference->IsOverwritable() && ImGui::Button("Apply"))
	{

		GameObject *to_reimport = GetPrefabParent(game_object);
		to_reimport->prefab_reference->Apply(to_reimport);
	}
	ImGui::SameLine();
	if (ImGui::Button("Revert"))
	{
		GameObject *to_reimport = GetPrefabParent(game_object);
		to_reimport->prefab_reference->Revert(to_reimport);
	}
}

GameObject* PanelGameObject::GetPrefabParent(GameObject* game_object)
{
	GameObject *to_reimport = game_object;
	bool prefab_parent = game_object->is_prefab_parent;
	while (to_reimport && !prefab_parent)
	{
		to_reimport = to_reimport->parent;
		prefab_parent = to_reimport->is_prefab_parent;
	}
	return to_reimport;
}