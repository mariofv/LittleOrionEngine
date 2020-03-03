#include "PanelComponent.h"

#include "Actions/EditorAction.h"
#include "Actions/EditorActionRotation.h"
#include "Actions/EditorActionScale.h"
#include "Actions/EditorActionTranslate.h"

#include "Component/ComponentCamera.h"
#include "Component/ComponentMeshRenderer.h"
#include "Component/ComponentTransform.h"
#include "Component/ComponentLight.h"

#include "Helper/Utils.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleActions.h"
#include "Module/ModuleEditor.h"

#include "UI/Panel/PanelPopups.h"
#include "UI/Panel/PopupsPanel/PanelPopupMeshSelector.h"

#include <imgui.h>
#include <imgui_stdlib.h>
#include <FontAwesome5/IconsFontAwesome5.h>

void PanelComponent::ShowComponentTransformWindow(ComponentTransform *transform)
{
	if (ImGui::CollapsingHeader(ICON_FA_RULER_COMBINED " Transform", ImGuiTreeNodeFlags_DefaultOpen))
	{

		if (ImGui::DragFloat3("Translation", transform->translation.ptr(), 0.01f))
		{
			transform->OnTransformChange();
		}
		//UndoRedo
		CheckClickForUndo(ModuleActions::UndoActionType::TRANSLATION, transform);

		if (ImGui::DragFloat3("Rotation", transform->rotation_degrees.ptr(), 0.1f, -180.f, 180.f))
		{
			transform->rotation = Utils::GenerateQuatFromDegFloat3(transform->rotation_degrees);
			transform->rotation_radians = Utils::Float3DegToRad(transform->rotation_degrees);
			transform->OnTransformChange();
		}
		//UndoRedo
		CheckClickForUndo(ModuleActions::UndoActionType::ROTATION, transform);

		if (ImGui::DragFloat3("Scale", transform->scale.ptr(), 0.01f))
		{
			transform->OnTransformChange();
		}

		//UndoRedo
		CheckClickForUndo(ModuleActions::UndoActionType::SCALE, transform);
	}
}

void PanelComponent::ShowComponentMeshRendererWindow(ComponentMeshRenderer *mesh)
{
	if (ImGui::CollapsingHeader(ICON_FA_SHAPES " Mesh Renderer", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if(ImGui::Checkbox("Active", &mesh->active))
		{
			//UndoRedo
			App->actions->action_component = mesh;
			App->actions->AddUndoAction(ModuleActions::UndoActionType::ENABLE_DISABLE_COMPONENT);
		}
		ImGui::SameLine();
		if (ImGui::Button("Delete"))
		{
			App->actions->DeleteComponentUndo(mesh);
			return;
		}
		ImGui::Separator();

		ImGui::AlignTextToFramePadding();
		ImGui::Text("Mesh");
		ImGui::SameLine();
		if (ImGui::Button(mesh->mesh_to_render->exported_file.c_str()))
		{
			App->editor->popups->mesh_selector_popup.show_mesh_selector_popup = true;
		}

		ImGui::AlignTextToFramePadding();
		ImGui::Text("Material");
		ImGui::SameLine();
		if (ImGui::Button(mesh->material_to_render->exported_file.c_str()))
		{
			App->editor->popups->material_selector_popup.show_material_selector_popup = true;
		}

		char tmp_string[16];
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Triangles");
		ImGui::SameLine();
		sprintf(tmp_string, "%d", mesh->mesh_to_render->vertices.size() / 3);
		ImGui::Button(tmp_string);

		ImGui::AlignTextToFramePadding();
		ImGui::Text("Vertices");
		ImGui::SameLine();
		sprintf(tmp_string, "%d", mesh->mesh_to_render->vertices.size());
		ImGui::Button(tmp_string);
	}
}

void PanelComponent::ShowComponentCameraWindow(ComponentCamera *camera)
{
	if (ImGui::CollapsingHeader(ICON_FA_VIDEO " Camera", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if(ImGui::Checkbox("Active", &camera->active))
		{
			//UndoRedo
			App->actions->action_component = camera;
			App->actions->AddUndoAction(ModuleActions::UndoActionType::ENABLE_DISABLE_COMPONENT);
		}
		ImGui::SameLine();
		if (ImGui::Button("Delete"))
		{
			App->actions->DeleteComponentUndo(camera);
			return;
		}
		ImGui::Separator();

		ImGui::InputFloat3("Front", &camera->camera_frustum.front[0], 3, ImGuiInputTextFlags_ReadOnly);
		ImGui::InputFloat3("Up", &camera->camera_frustum.up[0], 3, ImGuiInputTextFlags_ReadOnly);

		ImGui::Separator();

		ImGui::DragFloat("Mov Speed", &camera->camera_movement_speed, 0.01f ,camera->CAMERA_MINIMUN_MOVEMENT_SPEED, camera->CAMERA_MAXIMUN_MOVEMENT_SPEED);
		
		//UndoRedo
		CheckClickedCamera(camera);

		if (ImGui::DragFloat("FOV", &camera->camera_frustum.verticalFov, 0.01f, 0, 2 * 3.14f))
		{
			camera->SetFOV(camera->camera_frustum.verticalFov);
		}

		//UndoRedo
		CheckClickedCamera(camera);

		if (ImGui::DragFloat("Aspect Ratio", &camera->aspect_ratio, 0.01f , 0, 10))
		{
			camera->SetAspectRatio(camera->aspect_ratio);
		}

		//UndoRedo
		CheckClickedCamera(camera);

		if (ImGui::DragFloat("Near plane", &camera->camera_frustum.nearPlaneDistance, 0.01f, 1, camera->camera_frustum.farPlaneDistance + 1))
		{
			camera->SetNearDistance(camera->camera_frustum.nearPlaneDistance);
		}

		//UndoRedo
		CheckClickedCamera(camera);

		if (ImGui::DragFloat("Far plane", &camera->camera_frustum.farPlaneDistance, 0.01f, camera->camera_frustum.nearPlaneDistance + 1, camera->camera_frustum.nearPlaneDistance + 1000))
		{
			camera->SetFarDistance(camera->camera_frustum.farPlaneDistance);
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
				break;
			case 1:
				camera->SetClearMode(ComponentCamera::ClearMode::SKYBOX);
				break;
			}
		}		
		ImGui::ColorEdit3("Clear Color", camera->camera_clear_color);
		ImGui::Separator();

		if (ImGui::DragFloat("Orthographic Size", &camera->camera_frustum.orthographicHeight, 0.01f, 0, 100))
		{
			camera->SetOrthographicSize(float2(camera->camera_frustum.orthographicHeight * camera->aspect_ratio, camera->camera_frustum.orthographicHeight));
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

		ImGui::DragInt("Depth", &camera->depth, 0.05f);

		//UndoRedo
		CheckClickedCamera(camera);

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

void PanelComponent::ShowComponentLightWindow(ComponentLight *light)
{
	if (ImGui::CollapsingHeader(ICON_FA_LIGHTBULB " Light", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if(ImGui::Checkbox("Active", &light->active))
		{
			//UndoRedo
			App->actions->action_component = light;
			App->actions->AddUndoAction(ModuleActions::UndoActionType::ENABLE_DISABLE_COMPONENT);
		}
		ImGui::SameLine();
		if (ImGui::Button("Delete"))
		{
			App->actions->DeleteComponentUndo(light);

			return;
		}
		ImGui::Separator();

		ImGui::ColorEdit3("Color", light->light_color);
		
		CheckClickForUndo(ModuleActions::UndoActionType::EDIT_COMPONENTLIGHT, light);
		
		ImGui::DragFloat("Intensity ", &light->light_intensity, 0.01f, 0.f, 1.f);

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
			ImGui::DragFloat("Range", &light->point_light_parameters.range, 10.0f, 7.f, 3250.f);
			light->point_light_parameters.ChangePointLightAttenuationValues(light->point_light_parameters.range);
		}
		if (light->light_type == ComponentLight::LightType::SPOT_LIGHT)
		{
			if (ImGui::DragFloat("Spot Angle", &light->spot_light_parameters.spot_angle, 1.f, 1.f, 179.f))
			{
				light->spot_light_parameters.SetSpotAngle(light->spot_light_parameters.spot_angle);
			}
			if (ImGui::DragFloat("Edge Softness", &light->spot_light_parameters.edge_softness, 0.01f, 0.f, 1.f))
			{
				light->spot_light_parameters.SetEdgeSoftness(light->spot_light_parameters.edge_softness);
			}
			ImGui::DragFloat("Range", &light->spot_light_parameters.range, 10.0f, 7.f, 3250.f);
			light->spot_light_parameters.ChangeSpotLightAttenuationValues(light->spot_light_parameters.range);
		}
		
	}
}


void PanelComponent::ShowAddNewComponentButton()
{
	float window_width = ImGui::GetWindowWidth();
	float button_width = 0.5f * window_width;
	ImGui::SetCursorPosX((window_width - button_width) / 2.f);
	ImGui::Button("Add component", ImVec2(button_width, 25));

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

		ImGui::EndPopup();
	}

	if(component != nullptr)
	{
		App->actions->action_component = component;
		App->actions->AddUndoAction(ModuleActions::UndoActionType::ADD_COMPONENT);
	}
}

