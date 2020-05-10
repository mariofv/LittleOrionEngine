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
#include "Component/ComponentBoxCollider.h"
#include "Component/ComponentButton.h"
#include "Component/ComponentCamera.h"
#include "Component/ComponentCanvas.h"
#include "Component/ComponentCapsuleCollider.h"
#include "Component/ComponentCollider.h"
#include "Component/ComponentCylinderCollider.h"
#include "Component/ComponentImage.h"
#include "Component/ComponentMeshCollider.h"
#include "Component/ComponentMeshRenderer.h"
#include "Component/ComponentLight.h"
#include "Component/ComponentProgressBar.h"
#include "Component/ComponentScript.h"
#include "Component/ComponentSphereCollider.h"
#include "Component/ComponentText.h"
#include "Component/ComponentTransform.h"
#include "Component/ComponentTransform2D.h"
#include "Component/ComponentUI.h"

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

void PanelComponent::ShowComponentTransformWindow(ComponentTransform *transform)
{
	if (ImGui::CollapsingHeader(ICON_FA_RULER_COMBINED " Transform", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if ((transform->owner->GetComponent(Component::ComponentType::UI) != nullptr)) //Render transform 2d
		{
			ComponentTransform2D* transform_2d = &transform->owner->transform_2d;

			if (ImGui::DragFloat2("Position", transform_2d->position.ptr(), 1.0f))
			{
				transform_2d->OnTransformChange();
				transform_2d->modified_by_user = true;
			}

			ImGui::SetNextItemWidth(ImGui::GetWindowWidth() / 3);
			if (ImGui::DragFloat("Width", &transform_2d->width, 1))
			{
				transform_2d->OnTransformChange();
				transform_2d->modified_by_user = true;
			}

			ImGui::SameLine();
			ImGui::SetNextItemWidth(ImGui::GetWindowWidth() / 3);
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2);

			if (ImGui::DragFloat("Height", &transform_2d->height, 1))
			{
				transform_2d->OnTransformChange();
				transform_2d->modified_by_user = true;
			}
			//UndoRedo
			CheckClickForUndo(ModuleActions::UndoActionType::EDIT_RECT2D, transform_2d);

			if (ImGui::DragFloat("Rotation", &transform_2d->rotation, 0.1f, -180.f, 180.f))
			{
				transform_2d->OnTransformChange();
				transform_2d->modified_by_user = true;
			}
			//UndoRedo
			CheckClickForUndo(ModuleActions::UndoActionType::EDIT_RECT2D_ROTATION, transform_2d);

			if (ImGui::DragFloat2("Scale", transform_2d->scale.ptr(), 0.1f))
			{
				transform_2d->OnTransformChange();
				transform_2d->modified_by_user = true;
			}
		}
		else //Render transform 3d
		{
			if (ImGui::DragFloat3("Translation", transform->translation.ptr(), 0.01f))
			{
				transform->OnTransformChange();
				transform->modified_by_user = true;
			}
			//UndoRedo
			CheckClickForUndo(ModuleActions::UndoActionType::TRANSLATION, transform);
			
			if (ImGui::DragFloat3("Rotation", transform->rotation_degrees.ptr(), 0.1f, -180.f, 180.f))
			{
				transform->rotation = Utils::GenerateQuatFromDegFloat3(transform->rotation_degrees);
				transform->rotation_radians = Utils::Float3DegToRad(transform->rotation_degrees);
				transform->OnTransformChange();
				transform->modified_by_user = true;
			}
			//UndoRedo
			CheckClickForUndo(ModuleActions::UndoActionType::ROTATION, transform);
			
			if (ImGui::DragFloat3("Scale", transform->scale.ptr(), 0.01f))
			{
				transform->OnTransformChange();
				transform->modified_by_user = true;
			}
			//UndoRedo
			CheckClickForUndo(ModuleActions::UndoActionType::SCALE, transform);
		}
	}
}

void PanelComponent::ShowComponentMeshRendererWindow(ComponentMeshRenderer *mesh_renderer)
{
	if (ImGui::CollapsingHeader(ICON_FA_SHAPES " Mesh Renderer", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::Checkbox("Active", &mesh_renderer->active))
		{
			//UndoRedo
			App->actions->action_component = mesh_renderer;
			App->actions->AddUndoAction(ModuleActions::UndoActionType::ENABLE_DISABLE_COMPONENT);
			mesh_renderer->modified_by_user = true;
		}
		ImGui::SameLine();
		if (ImGui::Button("Delete"))
		{
			App->actions->DeleteComponentUndo(mesh_renderer);
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
			sprintf_s(tmp_string, "%d", mesh_renderer->mesh_to_render->indices.size() / 3);
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
		if (ImGui::Checkbox("Active", &camera->active))
		{
			//UndoRedo
			App->actions->action_component = camera;
			App->actions->AddUndoAction(ModuleActions::UndoActionType::ENABLE_DISABLE_COMPONENT);
			camera->modified_by_user = true;
		}
		ImGui::SameLine();
		if (ImGui::Button("Delete"))
		{
			App->actions->DeleteComponentUndo(camera);
			return;
		}
		ImGui::Separator();

		if (ImGui::InputFloat3("Front", &camera->camera_frustum.front[0], 3, ImGuiInputTextFlags_ReadOnly)) { camera->modified_by_user = true; }
		if (ImGui::InputFloat3("Up", &camera->camera_frustum.up[0], 3, ImGuiInputTextFlags_ReadOnly)) { camera->modified_by_user = true; }

		ImGui::Separator();

		if (ImGui::DragFloat("Mov Speed", &camera->camera_movement_speed, 0.01f, camera->CAMERA_MINIMUN_MOVEMENT_SPEED, camera->CAMERA_MAXIMUN_MOVEMENT_SPEED)) { camera->modified_by_user = true; }

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
			default:
				break;
			}
		}
		if (ImGui::ColorEdit3("Clear Color", camera->camera_clear_color)) { camera->modified_by_user = true; }
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

		if (ImGui::DragInt("Depth", &camera->depth, 0.05f)) { camera->modified_by_user = true; }

		//UndoRedo
		CheckClickedCamera(camera);

	}
}

void PanelComponent::ShowComponentLightWindow(ComponentLight *light)
{
	if (ImGui::CollapsingHeader(ICON_FA_LIGHTBULB " Light", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::Checkbox("Active", &light->active))
		{
			//UndoRedo
			App->actions->action_component = light;
			App->actions->AddUndoAction(ModuleActions::UndoActionType::ENABLE_DISABLE_COMPONENT);
			light->modified_by_user = true;
		}
		ImGui::SameLine();
		if (ImGui::Button("Delete"))
		{
			App->actions->DeleteComponentUndo(light);

			return;
		}
		ImGui::Separator();

		if (ImGui::ColorEdit3("Color", light->light_color)) { light->modified_by_user = true; }

		CheckClickForUndo(ModuleActions::UndoActionType::EDIT_COMPONENTLIGHT, light);

		if (ImGui::DragFloat("Intensity ", &light->light_intensity, 0.01f, 0.f, 1.f)) { light->modified_by_user = true; }

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
			default:
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
		if (ImGui::Checkbox("Active", &animation->active))
		{
			//UndoRedo
			App->actions->action_component = animation;
			App->actions->AddUndoAction(ModuleActions::UndoActionType::ENABLE_DISABLE_COMPONENT);
		}
		ImGui::SameLine();
		if (ImGui::Button("Delete"))
		{
			App->actions->DeleteComponentUndo(animation);

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
		if (ImGui::Checkbox("Playing", &animation->playing))
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
		if (ImGui::Checkbox("Active", &component_script->active))
		{
			//UndoRedo TODO
			//App->editor->action_component = component_script;
			//App->editor->AddUndoAction(ModuleEditor::UndoActionType::ENABLE_DISABLE_COMPONENT);
		}


		if (ImGui::Button("Delete"))
		{
			component_script->owner->RemoveComponent(component_script);
			App->scripts->RemoveComponentScript(component_script);
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

void PanelComponent::ShowComponentUIWindow(ComponentUI *ui)
{
	switch (ui->ui_type) 
	{
		case ComponentUI::UIType::CANVAS :
			ShowComponentCanvasWindow(static_cast<ComponentCanvas*>(ui));
			break;
		case ComponentUI::UIType::IMAGE:
			ShowComponentImageWindow(static_cast<ComponentImage*>(ui));
			break;
		case ComponentUI::UIType::TEXT:
			ShowComponentTextWindow(static_cast<ComponentText*>(ui));
			break;
		case ComponentUI::UIType::BUTTON:
			ShowComponentButtonWindow(static_cast<ComponentButton*>(ui));
			break;
		case ComponentUI::UIType::PROGRESSBAR:
			ShowComponentProgressBarWindow(static_cast<ComponentProgressBar*>(ui));
			break;
	}
}


void PanelComponent::ShowComponentColliderWindow(ComponentCollider* collider)
{
	switch (collider->collider_type)
	{
	case ComponentCollider::ColliderType::BOX:
		ShowComponentBoxColliderWindow(static_cast<ComponentBoxCollider*>(collider));
		break;
	case ComponentCollider::ColliderType::CAPSULE:
		ShowComponentCapsuleColliderWindow(static_cast<ComponentCapsuleCollider*>(collider));
		break;
	case ComponentCollider::ColliderType::SPHERE:
		ShowComponentSphereColliderWindow(static_cast<ComponentSphereCollider*>(collider));
		break;
	case ComponentCollider::ColliderType::CYLINDER:
		ShowComponentCylinderColliderWindow(static_cast<ComponentCylinderCollider*>(collider));
		break;
	case ComponentCollider::ColliderType::MESH:
		ShowComponentMeshColliderWindow(static_cast<ComponentMeshCollider*>(collider));
		break;
	}
}

void PanelComponent::ShowComponentCanvasWindow(ComponentCanvas *canvas)
{
	if (ImGui::CollapsingHeader(ICON_FA_PALETTE " Canvas", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ShowCommonUIWindow(canvas);
	}
}

void PanelComponent::ShowComponentImageWindow(ComponentImage* image) {
	if (ImGui::CollapsingHeader(ICON_FA_PALETTE " Image", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ShowCommonUIWindow(image);
	}
}


void PanelComponent::ShowComponentProgressBarWindow(ComponentProgressBar* progress_bar) {
	if (ImGui::CollapsingHeader(ICON_FA_PALETTE " Progress Bar", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ShowCommonUIWindow(progress_bar);
		ImGui::Separator();
		ImGui::DragFloat("Bar Value", &progress_bar->percentage, 0.1F, 0.0F, 100.0F);
		ImGui::InputInt("Bar Image", (int*)(&progress_bar->bar_texture));
		ImGui::ColorPicker3("Bar Color", progress_bar->bar_color.ptr());
	}
}

void PanelComponent::ShowComponentTextWindow(ComponentText* text)
{
	if (ImGui::CollapsingHeader(ICON_FA_PALETTE " Text", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ShowCommonUIWindow(text);
		ImGui::Separator();		
		ImGui::InputText("Text", &text->text);
		ImGui::Separator();
		if (ImGui::DragFloat("Font Size", (float*)(&text->scale)))
		{
			text->modified_by_user = true;
		}
		
	}
}
void PanelComponent::ShowComponentButtonWindow(ComponentButton *button)
{
	if (ImGui::CollapsingHeader(ICON_FA_PALETTE " Button", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ShowCommonUIWindow(button);
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
	float button_width = 0.5F * window_width;
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
		sprintf_s(tmp_string, "%s Box Collider", ICON_FA_BOX);
		if (ImGui::Selectable(tmp_string))
		{
			component = App->editor->selected_game_object->CreateComponent(ComponentCollider::ColliderType::BOX);

		}
		sprintf_s(tmp_string, "%s Capsule Collider", ICON_FA_CAPSULES);
		if (ImGui::Selectable(tmp_string))
		{
			component = App->editor->selected_game_object->CreateComponent(ComponentCollider::ColliderType::CAPSULE);
		}
		sprintf_s(tmp_string, "%s Cylinder Collider", ICON_FA_COLUMNS);
		if (ImGui::Selectable(tmp_string))
		{
			component = App->editor->selected_game_object->CreateComponent(ComponentCollider::ColliderType::CYLINDER);
		}
		sprintf_s(tmp_string, "%s Sphere Collider", ICON_FA_BASKETBALL_BALL);
		if (ImGui::Selectable(tmp_string))
		{
			component = App->editor->selected_game_object->CreateComponent(ComponentCollider::ColliderType::SPHERE);
		}
		sprintf_s(tmp_string, "%s Mesh Collider", ICON_FA_BORDER_NONE);
		if (ImGui::Selectable(tmp_string))
		{
			component = App->editor->selected_game_object->CreateComponent(ComponentCollider::ColliderType::MESH);
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

void PanelComponent::ShowCommonUIWindow(ComponentUI* ui)
{
	if (ImGui::Checkbox("Active", &ui->active))
	{
		//UndoRedo
		App->actions->action_component = ui;
		App->actions->AddUndoAction(ModuleActions::UndoActionType::ENABLE_DISABLE_COMPONENT);
	}
	ImGui::SameLine();
	if (ImGui::Button("Delete"))
	{
		App->actions->DeleteComponentUndo(ui);
		return;
	}
	ImGui::Separator();
	if (ImGui::DragInt("Layer", &ui->layer, 1.0F, -MAX_NUM_LAYERS, MAX_NUM_LAYERS))
	{
		//ui->owner->transform_2d.OnTransformChange();
		App->ui->SortComponentsUI();
	}
	ImGui::Separator();
	ImGui::InputInt("Texture", (int*)(&ui->ui_texture));
	uint32_t selected_resource = ImGui::ResourceDropper<Texture>();
	if (selected_resource != 0)
	{
		ui->SetTextureToRender(selected_resource);
	}

	ImGui::ColorPicker3("Color", ui->color.ptr());
}

bool PanelComponent::ShowCommonColliderWindow(ComponentCollider* collider)
{
	if (ImGui::Checkbox("Active", &collider->active))
	{
		//UndoRedo
		collider->DisablePhysics();
		App->actions->action_component = collider;
		App->actions->AddUndoAction(ModuleActions::UndoActionType::ENABLE_DISABLE_COMPONENT);
	}
	ImGui::SameLine();
	if (ImGui::Button("Delete"))
	{
		App->actions->DeleteComponentUndo(collider);
		return false;
	}
	ImGui::Separator();

	if (ImGui::DragFloat("Mass", &collider->mass, 1.0F, 0.1F, 1000.F)) {
		collider->SetMass(collider->mass);
	}
	if (ImGui::DragFloat("Friction", &collider->friction, 1.0F, 0.1F, 1000.F)) {
		collider->UpdateFriction();
	}
	if (ImGui::DragFloat("Rolling friction", &collider->rolling_friction, 1.0F, 0.1F, 2000.F)) {
		collider->SetRollingFriction();
	}
	if (ImGui::Checkbox("Visualize", &collider->visualize))
	{
		collider->SetVisualization();
	}
	if (ImGui::Checkbox("Static", &collider->is_static))
	{
		collider->SetStatic();
	}
	if (ImGui::Checkbox("Collision animation", &collider->detect_collision))
	{
		collider->SetCollisionDetection();
	}
	if (ImGui::Checkbox("Disable Physics", &collider->disable_physics))
	{
		collider->DisablePhysics();
	}
	ImGui::Text("Axis Rotation");
	if (ImGui::Checkbox("X Axis", &collider->x_axis))
	{
		collider->SetRotationAxis();
	}
	if (ImGui::Checkbox("Y Axis", &collider->y_axis))
	{
		collider->SetRotationAxis();
	}
	if (ImGui::Checkbox("Z Axis", &collider->z_axis))
	{
		collider->SetRotationAxis();
	}
	return true;
}


void PanelComponent::ShowComponentBoxColliderWindow(ComponentBoxCollider* box_collider)
{
	if (ImGui::CollapsingHeader(ICON_FA_BOX " Box Collider", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ShowCommonColliderWindow(box_collider))
		{
			if (ImGui::DragFloat3("Scale", box_collider->scale.ptr()))
			{
				box_collider->Scale();
			}
		}
	}
}

void PanelComponent::ShowComponentCapsuleColliderWindow(ComponentCapsuleCollider* capsule_collider)
{
	if (ImGui::CollapsingHeader(ICON_FA_CAPSULES " Capsule Collider", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if(ShowCommonColliderWindow(capsule_collider))
		{
			if (ImGui::DragFloat("Radius", &capsule_collider->scale.x))
			{
				capsule_collider->Scale();
			}
			if (ImGui::DragFloat("Height", &capsule_collider->scale.y))
			{
				capsule_collider->Scale();
			}
		}
	}
}

void PanelComponent::ShowComponentSphereColliderWindow(ComponentSphereCollider* sphere_collider)
{
	if (ImGui::CollapsingHeader(ICON_FA_BASKETBALL_BALL " Sphere Collider", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ShowCommonColliderWindow(sphere_collider))
		{
			if (ImGui::DragFloat("Radius", &sphere_collider->scale.x))
			{
				sphere_collider->Scale();
			}
		}
	}
}

void PanelComponent::ShowComponentCylinderColliderWindow(ComponentCylinderCollider* cylinder_collider)
{
	if (ImGui::CollapsingHeader(ICON_FA_COLUMNS " Cylinder Collider", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if(ShowCommonColliderWindow(cylinder_collider))
		{
			if (ImGui::DragFloat("Radius", &cylinder_collider->scale.x))
			{
				cylinder_collider->Scale();
			}
			if (ImGui::DragFloat("Height", &cylinder_collider->scale.y))
			{
				cylinder_collider->Scale();
			}
		}
	}
}

void PanelComponent::ShowComponentMeshColliderWindow(ComponentMeshCollider* mesh_collider)
{
	if (ImGui::CollapsingHeader(ICON_FA_BORDER_NONE " Mesh Collider", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ShowCommonColliderWindow(mesh_collider))
		{
			if (ImGui::DragFloat3("Scale", mesh_collider->scale.ptr()))
			{
				mesh_collider->Scale();
			}
		}
	}
}
