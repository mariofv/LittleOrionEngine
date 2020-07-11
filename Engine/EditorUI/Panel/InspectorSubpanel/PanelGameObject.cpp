#include "PanelGameObject.h"

#include "Helper/TagManager.h"

#include "Component/ComponentAnimation.h"
#include "Component/ComponentAudioListener.h"
#include "Component/ComponentAudioSource.h"
#include "Component/ComponentBillboard.h"
#include "Component/ComponentBoxCollider.h"
#include "Component/ComponentButton.h"
#include "Component/ComponentCamera.h"
#include "Component/ComponentCanvas.h"
#include "Component/ComponentCanvasRenderer.h"
#include "Component/ComponentCapsuleCollider.h"
#include "Component/ComponentEventSystem.h"
#include "Component/ComponentImage.h"
#include "Component/ComponentLight.h"
#include "Component/ComponentMeshRenderer.h"
#include "Component/ComponentParticleSystem.h"
#include "Component/ComponentScript.h"
#include "Component/ComponentSpriteMask.h"
#include "Component/ComponentText.h"
#include "Component/ComponentTransform.h"
#include "Component/ComponentTrail.h"

#include "EditorUI/Panel/PanelInspector.h"
#include "EditorUI/Panel/InspectorSubpanel/PanelTransform.h"
#include "EditorUI/Panel/PanelPopups.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleEditor.h"
#include "Module/ModuleScene.h"
#include "Module/ModuleResourceManager.h"
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
	focused = ImGui::IsWindowFocused();
	App->actions->active_macros = true;
	if (focused)
	{
		App->actions->active_macros = false;
	}

	ImGui::PushID(static_cast<int>(game_object->UUID));

	if (ImGui::Checkbox("###State", &game_object->active))
	{
		game_object->SetEnabled(game_object->active);
		game_object->modified_by_user = true;
	}

	ImGui::SameLine();
	ImGui::Text(ICON_FA_CUBE);

	ImGui::SameLine();
	if (ImGui::InputText("###GameObject name Input", &game_object->name))
	{
		App->actions->active_macros = false;
		game_object->modified_by_user = true;
	}

	ImGui::SameLine();
	if (ImGui::Checkbox("Static", &game_object->is_static))
	{
		game_object->SetStatic(game_object->is_static);
		game_object->modified_by_user = true;
	}

	ImGui::Spacing();
	std::string tag_name = game_object->tag != "" ? game_object->tag : "Untagged";
	if (ImGui::BeginCombo("Tag", tag_name.c_str()))
	{
		for (auto& tag_name : App->editor->tag_manager->tags) 
		{
			if (ImGui::Selectable(tag_name.c_str()))
			{
				game_object->tag = tag_name;
				game_object->modified_by_user = true;
			}
		}
		ImGui::Separator();
		if (ImGui::Selectable("Add Tag..."))
		{
			App->editor->popups->add_tag_popup_shown = true;
		}

		ImGui::EndCombo();
	}

	ImGui::Spacing();
	if (game_object->prefab_reference != nullptr)
	{
		ShowPrefabMenu(game_object);
	}
	ImGui::Separator();
	ImGui::Spacing();

	if (game_object->GetTransformType() == Component::ComponentType::TRANSFORM)
	{
		PanelTransform::ShowComponentTransformWindow(&game_object->transform);
	}
	else
	{
		PanelTransform::ShowComponentTransform2DWindow(&game_object->transform_2d);
	}

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

			case Component::ComponentType::CANVAS_RENDERER:
				component_panel.ShowComponentCanvasRendererWindow(static_cast<ComponentCanvasRenderer*>(component));
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

			case Component::ComponentType::ANIMATION:
				component_panel.ShowComponentAnimationWindow(static_cast<ComponentAnimation*>(component));
				break;

			case Component::ComponentType::BILLBOARD:
				component_panel.ShowComponentBillboard(static_cast<ComponentBillboard*>(component));
				break;

			case Component::ComponentType::PARTICLE_SYSTEM:
				component_panel.ShowComponentParticleSystem(static_cast<ComponentParticleSystem*>(component));
				break;

			case Component::ComponentType::TRAIL:
				component_panel.ShowComponentTrail(static_cast<ComponentTrail*>(component));
				break;

			case Component::ComponentType::CANVAS:
				component_panel.ShowComponentCanvasWindow(static_cast<ComponentCanvas*>(component));
				break;

			case Component::ComponentType::UI_IMAGE:
				component_panel.ShowComponentImageWindow(static_cast<ComponentImage*>(component));
				break;

			case Component::ComponentType::UI_SPRITE_MASK:
				component_panel.ShowComponentSpriteMaskWindow(static_cast<ComponentSpriteMask*>(component));
				break;

			case Component::ComponentType::UI_TEXT:
				component_panel.ShowComponentTextWindow(static_cast<ComponentText*>(component));
				break;

			case Component::ComponentType::UI_BUTTON:
				component_panel.ShowComponentButtonWindow(static_cast<ComponentButton*>(component));
				break;

			case Component::ComponentType::EVENT_SYSTEM:
				component_panel.ShowComponentEventSystem(static_cast<ComponentEventSystem*>(component));
				break;

			case Component::ComponentType::COLLIDER:
				component_panel.ShowComponentColliderWindow(static_cast<ComponentCollider*>(component));
				break;

			case Component::ComponentType::AUDIO_SOURCE:
				component_panel.ShowComponentAudioSourceWindow(static_cast<ComponentAudioSource*>(component));
				break;

			case Component::ComponentType::AUDIO_LISTENER:
				component_panel.ShowComponentAudioListenerWindow(static_cast<ComponentAudioListener*>(component));
				break;

			default:
				break;
		}

		ImGui::PopID();
	}

	ComponentMeshRenderer* mesh_renderer_component = static_cast<ComponentMeshRenderer*>(game_object->GetComponent(Component::ComponentType::MESH_RENDERER));
	if (mesh_renderer_component != nullptr && mesh_renderer_component->material_to_render != nullptr)
	{
		App->editor->inspector->material_panel.Render(mesh_renderer_component->material_to_render);
	}


	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	component_panel.ShowAddNewComponentButton();

	ImGui::PopID();
}

void PanelGameObject::ShowPrefabMenu(GameObject* game_object)
{
	ImGui::SameLine();
	if(game_object->prefab_reference->IsOverwritable() && ImGui::Button("Apply"))
	{
		GameObject *to_reimport = game_object->GetPrefabParent();
		to_reimport->prefab_reference->Apply(to_reimport);
		App->resources->Save<Prefab>(to_reimport->prefab_reference);
	}
	ImGui::SameLine();
	if (ImGui::Button("Revert"))
	{
		GameObject *to_reimport = game_object->GetPrefabParent();
		to_reimport->prefab_reference->Revert(to_reimport);
	}
}
