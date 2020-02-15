#include "PanelGameObject.h"

#include "Component/ComponentCamera.h"
#include "Component/ComponentMaterial.h"
#include "Component/ComponentMesh.h"
#include "Component/ComponentLight.h"
#include "Main/GameObject.h"

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
			case Component::ComponentType::MATERIAL:
				component_panel.ShowComponentMaterialWindow(static_cast<ComponentMaterial*>(component));
				break;
			case Component::ComponentType::MESH:
				component_panel.ShowComponentMeshWindow(static_cast<ComponentMesh*>(component));
				break;
			case Component::ComponentType::LIGHT:
				component_panel.ShowComponentLightWindow(static_cast<ComponentLight*>(component));
				break;
			default:
				break;
		}

		ImGui::PopID();
	}

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	component_panel.ShowAddNewComponentButton();
}