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
#include "Component/ComponentAudioSource.h"
#include "Component/ComponentButton.h"
#include "Component/ComponentCamera.h"
#include "Component/ComponentCanvas.h"
#include "Component/ComponentImage.h"
#include "Component/ComponentMeshRenderer.h"
#include "Component/ComponentLight.h"
#include "Component/ComponentParticleSystem.h"
#include "Component/ComponentProgressBar.h"
#include "Component/ComponentScript.h"
#include "Component/ComponentBillboard.h"
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

void PanelComponent::ShowComponentParticleSystem(ComponentParticleSystem* particle_system)
{
	
	if (ImGui::CollapsingHeader(ICON_FA_SQUARE " Particle System", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::Button("Delete"))
		{
			App->actions->DeleteComponentUndo(particle_system);
			return;
		}
		if (ImGui::CollapsingHeader("Particle Values", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Text("Texture");
			ImGui::SameLine();

			std::string texture_name = particle_system->billboard->billboard_texture == nullptr ? "None (Texture)" : App->resources->resource_DB->GetEntry(particle_system->billboard->billboard_texture->GetUUID())->resource_name;
			ImGuiID element_id = ImGui::GetID((std::to_string(particle_system->UUID) + "TextureSelector").c_str());
			if (ImGui::Button(texture_name.c_str()))
			{
				App->editor->popups->resource_selector_popup.ShowPanel(element_id, ResourceType::TEXTURE);
			}

			uint32_t selected_resource_uuid = App->editor->popups->resource_selector_popup.GetSelectedResource(element_id);
			if (selected_resource_uuid != 0)
			{
				particle_system->billboard->ChangeTexture(selected_resource_uuid);
			}
			selected_resource_uuid = ImGui::ResourceDropper<Texture>();
			if (selected_resource_uuid != 0)
			{
				particle_system->billboard->ChangeTexture(selected_resource_uuid);
			}
			int alignment_type = static_cast<int>(particle_system->billboard->alignment_type);
			if (ImGui::Combo("Billboard type", &alignment_type, "View point\0Axial\0Spritesheet\0Not aligned")) {
				switch (alignment_type)
				{
				case 0:
					particle_system->billboard->ChangeBillboardType(ComponentBillboard::AlignmentType::VIEW_POINT);
					break;
				case 1:
					particle_system->billboard->ChangeBillboardType(ComponentBillboard::AlignmentType::AXIAL);
					break;
				case 2:
					particle_system->billboard->ChangeBillboardType(ComponentBillboard::AlignmentType::SPRITESHEET);
					break;
				case 3:
					particle_system->billboard->ChangeBillboardType(ComponentBillboard::AlignmentType::CROSSED);
					break;
				}
			}
			if (particle_system->billboard->alignment_type == ComponentBillboard::AlignmentType::SPRITESHEET) {
				ImGui::InputInt("Rows", &particle_system->billboard->x_tiles, 1);
				ImGui::InputInt("Columns", &particle_system->billboard->y_tiles, 1);
				ImGui::InputFloat("Speed", &particle_system->billboard->sheet_speed, 1);
				ImGui::Checkbox("Oriented to camera", &particle_system->billboard->oriented_to_camera);
			}
			ImGui::Checkbox("Loop", &particle_system->loop);
			ImGui::Spacing();
			
			ImGui::SetNextItemWidth(ImGui::GetWindowWidth() / 5);
			ImGui::DragFloat("Width", &particle_system->particles_width, 0.01f, 0.0f, 100.0F);
			ImGui::SameLine();
			ImGui::SetNextItemWidth(ImGui::GetWindowWidth() / 5);
			ImGui::DragFloat("Height", &particle_system->particles_height, 0.01f, 0.0f, 100.0F);
			ImGui::SameLine();
			if (particle_system->size_random)
			{
				ImGui::Spacing();
				ImGui::SetNextItemWidth(ImGui::GetWindowWidth() / 5);
				ImGui::DragInt("Min size", &particle_system->min_size_of_particle, 1, 0, 999);
				ImGui::SameLine();
				ImGui::SetNextItemWidth(ImGui::GetWindowWidth() / 5);
				ImGui::DragInt("Max size", &particle_system->max_size_of_particle, 1, 1, 1000);
				ImGui::SameLine();
			}
			
			ImGui::Checkbox("Rand size", &particle_system->size_random);

			ImGui::Spacing();

			ImGui::DragFloat("Velocity", &particle_system->velocity_particles, 0.01f, 0.0f, 100.0F);
			ImGui::Spacing();

			ImGui::DragFloat("Life (in seconds)", &particle_system->particles_life_time, 1.0F, 0.0F, 100.0F);
			ImGui::Spacing();

			/*ImGui::DragInt("Number of Particles", &particle_system->max_particles, 1.0F, 0.0F, 1000.0F);
			ImGui::Spacing();*/

			ImGui::DragFloat("Time Between Particles", &particle_system->time_between_particles, 0.1F, 0.0F, 10.0f);
			ImGui::Spacing();
			if (particle_system->enabled_random_x)
			{
				ImGui::DragInt("Max X range", &particle_system->max_range_random_x, 1.0F, 0, 1000);
				ImGui::DragInt("Min X range", &particle_system->min_range_random_x, 1.0F, -1000, 0);
			}
			else
			{
				ImGui::DragInt("X position", &particle_system->position_x, 1.0F, -100, 1000);
			}
			ImGui::SameLine();
			ImGui::Checkbox("Rand X", &particle_system->enabled_random_x);

			ImGui::Spacing();

			if (particle_system->enabled_random_z)
			{
				ImGui::DragInt("Max Z range", &particle_system->max_range_random_z, 1.0F, 0, 1000);
				ImGui::DragInt("Min Z range", &particle_system->min_range_random_z, 1.0F, -1000, 0);
			}
			else
			{
				ImGui::DragInt("Z position", &particle_system->position_z, 1.0F, -100, 1000);
			}
			ImGui::SameLine();
			ImGui::Checkbox("Rand Z", &particle_system->enabled_random_z);
		}
		if (ImGui::CollapsingHeader(ICON_FA_SQUARE "Color Over Time"))
		{
			ImGui::Checkbox("Fade", &particle_system->color_fade);
			if (particle_system->color_fade)
			{
				ImGui::DragFloat("Fade time", &particle_system->color_fade_time, 0.01f, 0.0f, 10.0F);
			}
			ImGui::ColorEdit4("Particle Color##2f", (float*)&particle_system->billboard->color, ImGuiColorEditFlags_Float );
			
			
		}
		
	}

}
void PanelComponent::ShowComponentBillboard(ComponentBillboard *billboard)
{
	if (ImGui::CollapsingHeader(ICON_FA_SQUARE " Billboard", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::Checkbox("Active", &billboard->active))
		{
			//UndoRedo
			App->actions->action_component = billboard;
			App->actions->AddUndoAction(ModuleActions::UndoActionType::ENABLE_DISABLE_COMPONENT);
			billboard->modified_by_user = true;
		}
		ImGui::SameLine();
		if (ImGui::Button("Delete"))
		{
			App->actions->DeleteComponentUndo(billboard);
			return;
		}
		ImGui::Separator();

		ImGui::AlignTextToFramePadding();
		

		ImGui::Text("Texture");
		ImGui::SameLine();

		std::string texture_name = billboard->billboard_texture == nullptr ? "None (Texture)" : App->resources->resource_DB->GetEntry(billboard->billboard_texture->GetUUID())->resource_name;
		ImGuiID element_id = ImGui::GetID((std::to_string(billboard->UUID) + "TextureSelector").c_str());
		if (ImGui::Button(texture_name.c_str()))
		{
			App->editor->popups->resource_selector_popup.ShowPanel(element_id, ResourceType::TEXTURE);
		}

		uint32_t selected_resource_uuid = App->editor->popups->resource_selector_popup.GetSelectedResource(element_id);
		if (selected_resource_uuid != 0)
		{
			billboard->ChangeTexture(selected_resource_uuid);
		}
		selected_resource_uuid = ImGui::ResourceDropper<Texture>();
		if (selected_resource_uuid != 0)
		{
			billboard->ChangeTexture(selected_resource_uuid);
		}
		int alignment_type = static_cast<int>(billboard->alignment_type);
		if (ImGui::Combo("Billboard type", &alignment_type, "View point\0Axial\0Spritesheet\0Not aligned")) {
			switch (alignment_type)
			{
			case 0:
				billboard->ChangeBillboardType(ComponentBillboard::AlignmentType::VIEW_POINT);
				break;
			case 1:
				billboard->ChangeBillboardType(ComponentBillboard::AlignmentType::AXIAL);
				break;
			case 2:
				billboard->ChangeBillboardType(ComponentBillboard::AlignmentType::SPRITESHEET);
				break;
			case 3:
				billboard->ChangeBillboardType(ComponentBillboard::AlignmentType::CROSSED);
				break;
			}
		}

		if (billboard->alignment_type == ComponentBillboard::AlignmentType::SPRITESHEET) {
			ImGui::InputInt("Rows", &billboard->x_tiles, 1);
			ImGui::InputInt("Columns", &billboard->y_tiles, 1);
			ImGui::InputFloat("Speed", &billboard->sheet_speed, 1);
			ImGui::Checkbox("Oriented to camera", &billboard->oriented_to_camera);
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

		if (ImGui::ColorEdit3("Color", light->light_color)) { light->modified_by_user = true; };

		CheckClickForUndo(ModuleActions::UndoActionType::EDIT_COMPONENTLIGHT, light);

		if (ImGui::DragFloat("Intensity ", &light->light_intensity, 0.01f, 0.f, 100.f)) { light->modified_by_user = true; };

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
		
		sprintf_s(tmp_string, "%s Billboard", ICON_FA_SQUARE);
		if (ImGui::Selectable(tmp_string))
		{
			App->editor->selected_game_object->CreateComponent(Component::ComponentType::BILLBOARD);


		}
		sprintf_s(tmp_string, "%s Particle System", ICON_FA_SQUARE);
		if (ImGui::Selectable(tmp_string))
		{
			App->editor->selected_game_object->CreateComponent(Component::ComponentType::PARTICLE_SYSTEM);
		}

		sprintf_s(tmp_string, "%s Animation", ICON_FA_PLAY_CIRCLE);
		if (ImGui::Selectable(tmp_string))
		{
			component = App->editor->selected_game_object->CreateComponent(Component::ComponentType::ANIMATION);

		}

		sprintf_s(tmp_string, "%s Audio Source", ICON_FA_AUDIO_DESCRIPTION);
		if (ImGui::Selectable(tmp_string))
		{
			component = App->editor->selected_game_object->CreateComponent(Component::ComponentType::AUDIO_SOURCE);

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

void PanelComponent::ShowComponentAudioSourceWindow(ComponentAudioSource* component_audio_source)
{
	if (ImGui::CollapsingHeader(ICON_FA_AUDIO_DESCRIPTION " Audio Source", ImGuiTreeNodeFlags_DefaultOpen))
	{
		//ImGui::AlignTextToFramePadding();
		ImGui::Checkbox("3D Sound", &component_audio_source->sound_3d);
		std::string soundbank_name = component_audio_source->soundbank == nullptr ? "None (Sound Bank)" : App->resources->resource_DB->GetEntry(component_audio_source->soundbank->GetUUID())->resource_name;
		ImGuiID element_id = ImGui::GetID((std::to_string(component_audio_source->UUID) + "SoundBankSelector").c_str());
		if (ImGui::Button(soundbank_name.c_str()))
		{
			App->editor->popups->resource_selector_popup.ShowPanel(element_id, ResourceType::SOUND);
		}
		uint32_t selected_resource = App->editor->popups->resource_selector_popup.GetSelectedResource(element_id);
		if (selected_resource != 0)
		{
			component_audio_source->SetSoundBank(selected_resource);
			component_audio_source->modified_by_user = true;
		}
		selected_resource = ImGui::ResourceDropper<StateMachine>();
		if (selected_resource != 0)
		{
			component_audio_source->SetSoundBank(selected_resource);
			component_audio_source->modified_by_user = true;
		}
		if (component_audio_source->soundbank)
		{
			static std::string soundbank;
			ImGui::InputText("SoundBank ", &soundbank);
			if (ImGui::Button("Play"))
			{
				component_audio_source->PlayEvent(soundbank);
			}
		}
		if (ImGui::SliderFloat("Volume", &component_audio_source->volume, 0, 30))
		{
			component_audio_source->SetVolume(component_audio_source->volume);
		}
	}
}
