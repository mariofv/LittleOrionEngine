#include "PanelComponent.h"

#include "Animation/AnimController.h"

#include "Actions/EditorAction.h"
#include "Actions/EditorActionRotation.h"
#include "Actions/EditorActionScale.h"
#include "Actions/EditorActionTranslate.h"

#include "EditorUI/Helper/ImGuiHelper.h"
#include "EditorUI/Panel/PanelPopups.h"
#include "Helper/Utils.h"

#include "Component/ComponentAnimation.h"
#include "Component/ComponentButton.h"
#include "Component/ComponentCamera.h"
#include "Component/ComponentCanvas.h"
#include "Component/ComponentCanvasRenderer.h"
#include "Component/ComponentImage.h"
#include "Component/ComponentMeshRenderer.h"
#include "Component/ComponentLight.h"
#include "Component/ComponentScript.h"
#include "Component/ComponentText.h"
#include "Component/ComponentTransform.h"
#include "Component/ComponentTransform2D.h"

#include "Helper/Utils.h"
#include "Math/Rect.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleActions.h"
#include "Module/ModuleEditor.h"
#include "Module/ModuleFileSystem.h"
#include "Module/ModuleResourceManager.h"
#include "Module/ModuleRender.h"
#include "Module/ModuleScriptManager.h"
#include "Module/ModuleSpacePartitioning.h"
#include "Module/ModuleUI.h"

#include "ResourceManagement/Importer/Importer.h"
#include "ResourceManagement/Resources/StateMachine.h"

#include <imgui.h>
#include <imgui_stdlib.h>
#include <FontAwesome5/IconsFontAwesome5.h>

void PanelComponent::ShowComponentMeshRendererWindow(ComponentMeshRenderer *mesh_renderer)
{
	if (ImGui::CollapsingHeader(ICON_FA_DRAW_POLYGON " Mesh Renderer", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (!ShowCommonComponentWindow(mesh_renderer))
		{
			return;
		}
		ImGui::Separator();


		ImGui::AlignTextToFramePadding();
		ImGui::Text("Mesh");
		ImGui::SameLine();
		std::string mesh_name = mesh_renderer->mesh_to_render == nullptr ? "None (Mesh)" : App->resources->resource_DB->GetEntry(mesh_renderer->mesh_to_render->GetUUID())->resource_name;
		ImGuiID element_id = ImGui::GetID((std::to_string(mesh_renderer->UUID) + "MeshSelector").c_str());
		if (ImGui::Button(mesh_name.c_str()))
		{
			App->editor->popups->resource_selector_popup.ShowPanel(element_id, ResourceType::MESH);
		}

		uint32_t selected_resource = App->editor->popups->resource_selector_popup.GetSelectedResource(element_id);
		if (selected_resource != 0)
		{
			mesh_renderer->SetMesh(selected_resource);
		}
		uint32_t incoming_mesh_uuid = ImGui::ResourceDropper<Mesh>();
		if (incoming_mesh_uuid != 0)
		{
			mesh_renderer->SetMesh(incoming_mesh_uuid);
			mesh_renderer->modified_by_user = true;
		}

		ImGui::AlignTextToFramePadding();
		ImGui::Text("Material");
		ImGui::SameLine();
		
		std::string material_name = mesh_renderer->material_to_render == nullptr ? "None (Material)" : App->resources->resource_DB->GetEntry(mesh_renderer->material_to_render->GetUUID())->resource_name;
		element_id = ImGui::GetID((std::to_string(mesh_renderer->UUID) + "MaterialSelector").c_str());
		if (ImGui::Button(material_name.c_str()))
		{
			App->editor->popups->resource_selector_popup.ShowPanel(element_id, ResourceType::MATERIAL);
		}
		selected_resource = App->editor->popups->resource_selector_popup.GetSelectedResource(element_id);
		if (selected_resource != 0)
		{
			mesh_renderer->SetMaterial(selected_resource);
		}
		uint32_t incoming_material_uuid = ImGui::ResourceDropper<Material>();
		if (incoming_material_uuid != 0)
		{
			mesh_renderer->SetMaterial(incoming_material_uuid);
			mesh_renderer->modified_by_user = true;
		}

		ImGui::AlignTextToFramePadding();
		ImGui::Text("Skeleton");
		ImGui::SameLine();

		std::string skeleton_name = mesh_renderer->skeleton == nullptr ? "None (Skeleton)" : App->resources->resource_DB->GetEntry(mesh_renderer->material_to_render->GetUUID())->resource_name;;
		element_id = ImGui::GetID((std::to_string(mesh_renderer->UUID) + "SkeletonSelector").c_str());
		if (ImGui::Button(skeleton_name.c_str()))
		{
			App->editor->popups->resource_selector_popup.ShowPanel(element_id, ResourceType::SKELETON);
		}
		selected_resource = App->editor->popups->resource_selector_popup.GetSelectedResource(element_id);
		if (selected_resource != 0)
		{
			mesh_renderer->SetSkeleton(selected_resource);
		}
		uint32_t incoming_skeleton_uuid = ImGui::ResourceDropper<Skeleton>();
		if (incoming_skeleton_uuid != 0)
		{
			mesh_renderer->SetSkeleton(incoming_skeleton_uuid);
			mesh_renderer->modified_by_user = true;
		}

		if (mesh_renderer->mesh_to_render != nullptr)
		{
			char tmp_string[16];
			ImGui::AlignTextToFramePadding();
			ImGui::Text("Triangles");
			ImGui::SameLine();
			sprintf_s(tmp_string, "%d", mesh_renderer->mesh_to_render->vertices.size() / 3);
			ImGui::Button(tmp_string);

			ImGui::AlignTextToFramePadding();
			ImGui::Text("Vertices");
			ImGui::SameLine();
			sprintf_s(tmp_string, "%d", mesh_renderer->mesh_to_render->vertices.size());
			ImGui::Button(tmp_string);
		}
	}
}

void PanelComponent::ShowComponentCameraWindow(ComponentCamera *camera)
{
	if (ImGui::CollapsingHeader(ICON_FA_VIDEO " Camera", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (!ShowCommonComponentWindow(camera))
		{
			return;
		}
		ImGui::Separator();

		if (ImGui::InputFloat3("Front", &camera->camera_frustum.front[0], 3, ImGuiInputTextFlags_ReadOnly)) { camera->modified_by_user = true; };
		if (ImGui::InputFloat3("Up", &camera->camera_frustum.up[0], 3, ImGuiInputTextFlags_ReadOnly)) { camera->modified_by_user = true; };

		ImGui::Separator();

		if (ImGui::DragFloat("Mov Speed", &camera->camera_movement_speed, 0.01f, camera->CAMERA_MINIMUN_MOVEMENT_SPEED, camera->CAMERA_MAXIMUN_MOVEMENT_SPEED)) { camera->modified_by_user = true; };

		//UndoRedo
		CheckClickedCamera(camera);

		if (ImGui::DragFloat("FOV", &camera->camera_frustum.verticalFov, 0.01f, 0, 2 * 3.14f))
		{
			camera->SetFOV(camera->camera_frustum.verticalFov);
			camera->modified_by_user = true;
		}

		//UndoRedo
		CheckClickedCamera(camera);

		if (ImGui::DragFloat("Aspect Ratio", &camera->aspect_ratio, 0.01f, 0, 10))
		{
			camera->SetAspectRatio(camera->aspect_ratio);
			camera->modified_by_user = true;
		}

		//UndoRedo
		CheckClickedCamera(camera);

		if (ImGui::DragFloat("Near plane", &camera->camera_frustum.nearPlaneDistance, 0.01f, 1, camera->camera_frustum.farPlaneDistance + 1))
		{
			camera->SetNearDistance(camera->camera_frustum.nearPlaneDistance);
			camera->modified_by_user = true;
		}

		//UndoRedo
		CheckClickedCamera(camera);

		if (ImGui::DragFloat("Far plane", &camera->camera_frustum.farPlaneDistance, 0.01f, camera->camera_frustum.nearPlaneDistance + 1, camera->camera_frustum.nearPlaneDistance + 1000))
		{
			camera->SetFarDistance(camera->camera_frustum.farPlaneDistance);
			camera->modified_by_user = true;
		}

		//UndoRedo
		CheckClickedCamera(camera);

		ImGui::Separator();
		int camera_clear_mode = static_cast<int>(camera->camera_clear_mode);
		if (ImGui::Combo("Clear Mode", &camera_clear_mode, "Color\0Skybox\0"))
		{
			switch (camera_clear_mode)
			{
			case 0:
				camera->SetClearMode(ComponentCamera::ClearMode::COLOR);
				camera->modified_by_user = true;
				break;
			case 1:
				camera->SetClearMode(ComponentCamera::ClearMode::SKYBOX);
				camera->modified_by_user = true;
				break;
			}
		}
		if (ImGui::ColorEdit3("Clear Color", camera->camera_clear_color)) { camera->modified_by_user = true; };
		ImGui::Separator();

		if (ImGui::DragFloat("Orthographic Size", &camera->camera_frustum.orthographicHeight, 0.01f, 0, 100))
		{
			camera->SetOrthographicSize(float2(camera->camera_frustum.orthographicHeight * camera->aspect_ratio, camera->camera_frustum.orthographicHeight));
			camera->modified_by_user = true;
		}

		//UndoRedo
		CheckClickedCamera(camera);

		if (ImGui::Combo("Front faces", &camera->perpesctive_enable, "Perspective\0Orthographic\0"))
		{
			switch (camera->perpesctive_enable)
			{
			case 0:
				camera->SetPerpesctiveView();
				break;
			case 1:
				camera->SetOrthographicView();
				break;
			}
		}
		ImGui::Separator();

		if (ImGui::DragInt("Depth", &camera->depth, 0.05f)) { camera->modified_by_user = true; };

		//UndoRedo
		CheckClickedCamera(camera);

	}
}

void PanelComponent::ShowComponentLightWindow(ComponentLight *light)
{
	if (ImGui::CollapsingHeader(ICON_FA_LIGHTBULB " Light", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (!ShowCommonComponentWindow(light))
		{
			return;
		}
		ImGui::Separator();

		if (ImGui::ColorEdit3("Color", light->light_color)) { light->modified_by_user = true; };

		CheckClickForUndo(ModuleActions::UndoActionType::EDIT_COMPONENTLIGHT, light);

		if (ImGui::DragFloat("Intensity ", &light->light_intensity, 0.01f, 0.f, 1.f)) { light->modified_by_user = true; };

		CheckClickForUndo(ModuleActions::UndoActionType::EDIT_COMPONENTLIGHT, light);

		int light_type = static_cast<int>(light->light_type);

		if (ImGui::Combo("Light Type", &light_type, "Point\0Spot\0Directional"))
		{
			switch (light_type)
			{
			case 0:
				light->light_type = ComponentLight::LightType::POINT_LIGHT;
				break;
			case 1:
				light->light_type = ComponentLight::LightType::SPOT_LIGHT;
				break;
			case 2:
				light->light_type = ComponentLight::LightType::DIRECTIONAL_LIGHT;
				break;
			}
		}
		if (light->light_type == ComponentLight::LightType::POINT_LIGHT)
		{
			if (ImGui::DragFloat("Range", &light->point_light_parameters.range, 1.f, 1.f, 100.f))
			{
				light->point_light_parameters.ChangePointLightAttenuationValues(light->point_light_parameters.range);
				light->modified_by_user = true;
			}
		}
		if (light->light_type == ComponentLight::LightType::SPOT_LIGHT)
		{
			if (ImGui::DragFloat("Spot Angle", &light->spot_light_parameters.spot_angle, 1.f, 1.f, 179.f))
			{
				light->spot_light_parameters.SetSpotAngle(light->spot_light_parameters.spot_angle);
				light->modified_by_user = true;
			}
			if (ImGui::DragFloat("Edge Softness", &light->spot_light_parameters.edge_softness, 0.01f, 0.f, 1.f))
			{
				light->spot_light_parameters.SetEdgeSoftness(light->spot_light_parameters.edge_softness);
				light->modified_by_user = true;
			}
			if (ImGui::DragFloat("Range", &light->spot_light_parameters.range, 1.f, 1.f, 100.f))
			{
				light->spot_light_parameters.ChangeSpotLightAttenuationValues(light->spot_light_parameters.range);
				light->modified_by_user = true;
			}
		}

	}
}

void PanelComponent::ShowComponentAnimationWindow(ComponentAnimation* animation)
{
	if (ImGui::CollapsingHeader(ICON_FA_PLAY_CIRCLE " Animation", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (!ShowCommonComponentWindow(animation))
		{
			return;
		}
		ImGui::SameLine();
		if (ImGui::Button("Reload"))
		{
			animation->SetStateMachine(animation->animation_controller->state_machine->GetUUID());
			animation->Init();
			return;
		}
		ImGui::Separator();

		ImGui::AlignTextToFramePadding();
		ImGui::Text("State Machine");
		ImGui::SameLine();

		std::string state_machine_name = animation->animation_controller->state_machine == nullptr ? "None (State machine)" : App->resources->resource_DB->GetEntry(animation->animation_controller->state_machine->GetUUID())->resource_name;
		ImGuiID element_id = ImGui::GetID((std::to_string(animation->UUID) + "StateMachineSelector").c_str());
		if (ImGui::Button(state_machine_name.c_str()))
		{
			App->editor->popups->resource_selector_popup.ShowPanel(element_id, ResourceType::STATE_MACHINE);
		}
		uint32_t selected_resource = App->editor->popups->resource_selector_popup.GetSelectedResource(element_id);
		if (selected_resource != 0)
		{
			animation->SetStateMachine(selected_resource);
			animation->modified_by_user = true;
		}
		selected_resource = ImGui::ResourceDropper<StateMachine>();
		if (selected_resource != 0)
		{
			animation->SetStateMachine(selected_resource);
			animation->modified_by_user = true;
		}
		
		if (animation->animation_controller->state_machine && animation->animation_controller->active_state)
		{
			ImGui::InputScalar("###Interpolation", ImGuiDataType_U64, &(animation->animation_controller->active_state->name_hash), nullptr, nullptr, nullptr, ImGuiInputTextFlags_ReadOnly);
			static std::string trigger;
			ImGui::InputText("Trigger ", &trigger);
			if (ImGui::Button("Activate"))
			{
				animation->ActiveAnimation(trigger);
			}
		}
		ImGui::AlignTextToFramePadding();
		ImGui::Separator();
		if (ImGui::Checkbox("Playing", &animation->playing));
		ImGui::SameLine();
		if (ImGui::Button("Play"))
		{
			animation->Play();
		}
		ImGui::SameLine();
		if (ImGui::Button("Stop"))
		{
			animation->Stop();
		}

		for (auto& playing_clip : animation->animation_controller->playing_clips)
		{
			if (!playing_clip.clip)
			{
				break;
			}
			ImGui::Checkbox("Loop", &(playing_clip.clip->loop));
			ImGui::SliderInt("Animation time", &playing_clip.current_time, 0, playing_clip.clip->animation_time);
		}

	}
}
void PanelComponent::ShowComponentScriptWindow(ComponentScript* component_script)
{
	if (ImGui::CollapsingHeader(ICON_FA_EDIT " Script", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (!ShowCommonComponentWindow(component_script))
		{
			return;
		}

		ImGui::SameLine();
		if (ImGui::Button("Refresh"))
		{
			App->scripts->Refresh();
			return;
		}
		ShowScriptsCreated(component_script);
		ImGui::Separator();

		component_script->ShowComponentWindow();

		// to implement CheckClickForUndo(ModuleEditor::UndoActionType::EDIT_COMPONENTSCRIPT, component_script);

	}
}

void PanelComponent::ShowComponentCanvasWindow(ComponentCanvas *canvas)
{
	if (ImGui::CollapsingHeader(ICON_FA_SQUARE " Canvas", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (!ShowCommonComponentWindow(canvas))
		{
			return;
		}
	}
}

void PanelComponent::ShowComponentCanvasRendererWindow(ComponentCanvasRenderer* canvas_renderer)
{
	if (ImGui::CollapsingHeader(ICON_FA_DOT_CIRCLE " Canvas Renderer", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (!ShowCommonComponentWindow(canvas_renderer))
		{
			return;
		}
	}
}

void PanelComponent::ShowComponentImageWindow(ComponentImage* component_image) {
	if (ImGui::CollapsingHeader(ICON_FA_IMAGE " Image", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (!ShowCommonComponentWindow(component_image))
		{
			return;
		}

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::AlignTextToFramePadding();
		ImGui::Text("Texture");
		ImGui::SameLine();

		std::string texture_name = component_image->texture_to_render == nullptr ? "None (Texture)" : App->resources->resource_DB->GetEntry(component_image->texture_to_render->GetUUID())->resource_name;
		ImGuiID element_id = ImGui::GetID((std::to_string(component_image->UUID) + "MeshSelector").c_str());
		if (ImGui::Button(texture_name.c_str()))
		{
			App->editor->popups->resource_selector_popup.ShowPanel(element_id, ResourceType::TEXTURE);
		}

		uint32_t selected_resource = App->editor->popups->resource_selector_popup.GetSelectedResource(element_id);
		if (selected_resource != 0)
		{
			component_image->SetTextureToRender(selected_resource);
		}
		selected_resource = ImGui::ResourceDropper<Texture>();
		if (selected_resource != 0)
		{
			component_image->SetTextureToRender(selected_resource);
		}

		ImGui::ColorPicker3("Color", component_image->color.ptr());
	}
}

void PanelComponent::ShowComponentTextWindow(ComponentText* text)
{
	if (ImGui::CollapsingHeader(ICON_FA_PALETTE " Text", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (!ShowCommonComponentWindow(text))
		{
			return;
		}
		ImGui::Separator();		
		if (ImGui::InputText("Text", &text->text))
		{
			text->SetText(text->text);
		}
		
		ImGui::Separator();
		ImGui::Text("Font");
		ImGui::SameLine();
		std::string font_name = text->font == nullptr ? "None (Font)" : App->resources->resource_DB->GetEntry(text->font->GetUUID())->resource_name;
		ImGuiID element_id = ImGui::GetID((std::to_string(text->UUID) + "FontSelector").c_str());
		if (ImGui::Button(font_name.c_str()))
		{
			App->editor->popups->resource_selector_popup.ShowPanel(element_id, ResourceType::FONT);
		}	
		uint32_t selected_resource = App->editor->popups->resource_selector_popup.GetSelectedResource(element_id);
		if (selected_resource != 0)
		{
			text->SetFont(selected_resource);
			text->modified_by_user = true;
		}
		selected_resource = ImGui::ResourceDropper<Font>();
		if (selected_resource != 0)
		{
			text->SetFont(selected_resource);
		}

		if (ImGui::DragFloat("Font Size", (float*)(&text->font_size)))
		{
			text->SetFontSize(text->font_size);
			text->modified_by_user = true;
		}
		
	}
}
void PanelComponent::ShowComponentButtonWindow(ComponentButton *button)
{
	if (ImGui::CollapsingHeader(ICON_FA_PALETTE " Button", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ShowCommonComponentWindow(button);
	}
}

void PanelComponent::CheckClickedCamera(ComponentCamera* camera)
{
	//UndoRedo
	if (ImGui::IsItemActive() && !ImGui::IsItemActiveLastFrame())
	{
		//Push new action
		App->actions->action_component = camera;
		App->actions->AddUndoAction(ModuleActions::UndoActionType::EDIT_COMPONENTCAMERA);
	}
}

void PanelComponent::CheckClickForUndo(ModuleActions::UndoActionType  type, Component* component)
{
	if (ImGui::IsItemActive() && !ImGui::IsItemActiveLastFrame())
	{
		switch (type)
		{
		case ModuleActions::UndoActionType::TRANSLATION:
			App->actions->previous_transform = ((ComponentTransform*)component)->GetTranslation();
			break;
		case ModuleActions::UndoActionType::ROTATION:
			App->actions->previous_transform = ((ComponentTransform*)component)->GetRotationRadiants();
			break;
		case ModuleActions::UndoActionType::SCALE:
			App->actions->previous_transform = ((ComponentTransform*)component)->GetScale();
			break;
		case ModuleActions::UndoActionType::EDIT_RECT2D:
		case ModuleActions::UndoActionType::EDIT_RECT2D_ROTATION:
			App->actions->action_component = (ComponentTransform2D*) component;
			break;
		case ModuleActions::UndoActionType::EDIT_COMPONENTLIGHT:
			App->actions->previous_light_color[0] = ((ComponentLight*)component)->light_color[0];
			App->actions->previous_light_color[1] = ((ComponentLight*)component)->light_color[1];
			App->actions->previous_light_color[2] = ((ComponentLight*)component)->light_color[2];
			App->actions->previous_light_intensity = ((ComponentLight*)component)->light_intensity;
			App->actions->action_component = component;
			break;
		default:
			break;
		}


		App->actions->clicked = true;
	}

	if (ImGui::IsItemDeactivatedAfterChange())
	{
		App->actions->AddUndoAction(type);
		App->actions->clicked = false;
	}

}

void PanelComponent::ShowAddNewComponentButton()
{
	float window_width = ImGui::GetWindowWidth();
	float button_width = 0.5f * window_width;
	ImGui::SetCursorPosX((window_width - button_width) / 2.f);
	ImGui::Button("Add Component", ImVec2(button_width, 25));

	//UndoRedo
	Component* component = nullptr;

	if (ImGui::BeginPopupContextItem("Add component", 0))
	{
		char tmp_string[128];

		sprintf_s(tmp_string, "%s Camera", ICON_FA_VIDEO);
		if (ImGui::Selectable(tmp_string))
		{
			component = App->editor->selected_game_object->CreateComponent(Component::ComponentType::CAMERA);

		}

		sprintf_s(tmp_string, "%s Light", ICON_FA_LIGHTBULB);
		if (ImGui::Selectable(tmp_string))
		{
			component = App->editor->selected_game_object->CreateComponent(Component::ComponentType::LIGHT);

		}
		sprintf_s(tmp_string, "%s Script", ICON_FA_EDIT);
		if (ImGui::Selectable(tmp_string))
		{
			component = App->editor->selected_game_object->CreateComponent(Component::ComponentType::SCRIPT);

		}

		sprintf_s(tmp_string, "%s Mesh Renderer", ICON_FA_DRAW_POLYGON);
		if (ImGui::Selectable(tmp_string))
		{
			component = App->editor->selected_game_object->CreateComponent(Component::ComponentType::MESH_RENDERER);

			if (!App->editor->selected_game_object->IsStatic())
			{
				App->space_partitioning->InsertAABBTree(App->editor->selected_game_object);
			}

		}
		sprintf_s(tmp_string, "%s Animation", ICON_FA_PLAY_CIRCLE);
		if (ImGui::Selectable(tmp_string))
		{
			component = App->editor->selected_game_object->CreateComponent(Component::ComponentType::ANIMATION);

		}

		sprintf_s(tmp_string, "%s Canvas", ICON_FA_SQUARE);
		if (ImGui::Selectable(tmp_string))
		{
			component = App->editor->selected_game_object->CreateComponent(Component::ComponentType::CANVAS);

		}

		sprintf_s(tmp_string, "%s Canvas Renderer", ICON_FA_DOT_CIRCLE);
		if (ImGui::Selectable(tmp_string))
		{
			component = App->editor->selected_game_object->CreateComponent(Component::ComponentType::CANVAS_RENDERER);

		}
		
		sprintf_s(tmp_string, "%s UI Image", ICON_FA_IMAGE);
		if (ImGui::Selectable(tmp_string))
		{
			component = App->editor->selected_game_object->CreateComponent(Component::ComponentType::UI_IMAGE);
		}
		ImGui::EndPopup();
	}

	if (component != nullptr)
	{
		component->added_by_user = true;
		App->actions->action_component = component;
		App->actions->AddUndoAction(ModuleActions::UndoActionType::ADD_COMPONENT);
	}
}

void PanelComponent::ShowScriptsCreated(ComponentScript* component_script)
{

	if (ImGui::BeginCombo("Add Script", component_script->name.c_str()))
	{
		for (auto& script_name : App->scripts->scripts_list) {
			if (ImGui::Selectable(script_name.c_str()))
			{
				component_script->LoadName(script_name);

			}
		}
		ImGui::Separator();
		if (ImGui::Selectable("Create new Script"))
		{
			App->editor->popups->create_script_shown = true;
		}

		ImGui::EndCombo();
	}
}	

ENGINE_API void PanelComponent::DropGOTarget(GameObject*& go)
{
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_GameObject"))
		{
			assert(payload->DataSize == sizeof(GameObject*));
			GameObject *incoming_game_object = *(GameObject**)payload->Data;
			go = incoming_game_object;
		}
		ImGui::EndDragDropTarget();
	}
}

bool PanelComponent::ShowCommonComponentWindow(Component* component)
{
	if (ImGui::Checkbox("Active", &component->active))
	{
		component->modified_by_user = true;
		if (component->active)
		{
			component->Enable();
		}
		else
		{
			component->Disable();
		}

		//UndoRedo
		App->actions->action_component = component;
		App->actions->AddUndoAction(ModuleActions::UndoActionType::ENABLE_DISABLE_COMPONENT);
	}

	ImGui::SameLine();
	if (ImGui::Button("Delete"))
	{
		App->actions->DeleteComponentUndo(component);
		return false;
	}

	return true;
}
