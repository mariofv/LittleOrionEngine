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
#include "Component/ComponentAudioListener.h"
#include "Component/ComponentAudioSource.h"
#include "Component/ComponentBillboard.h"
#include "Component/ComponentBoxCollider.h"
#include "Component/ComponentButton.h"
#include "Component/ComponentCamera.h"
#include "Component/ComponentCanvas.h"
#include "Component/ComponentCanvasRenderer.h"
#include "Component/ComponentCapsuleCollider.h"
#include "Component/ComponentCollider.h"
#include "Component/ComponentCylinderCollider.h"
#include "Component/ComponentEventSystem.h"
#include "Component/ComponentImage.h"
#include "Component/ComponentMeshCollider.h"
#include "Component/ComponentMeshRenderer.h"
#include "Component/ComponentLight.h"
#include "Component/ComponentParticleSystem.h"
#include "Component/ComponentScript.h"
#include "Component/ComponentSpriteMask.h"
#include "Component/ComponentSphereCollider.h"
#include "Component/ComponentText.h"
#include "Component/ComponentTransform.h"
#include "Component/ComponentTransform2D.h"

#include "Helper/Utils.h"

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

#include "PanelParticleSystem.h"

#include "ResourceManagement/Importer/Importer.h"
#include "ResourceManagement/Resources/StateMachine.h"

#include <imgui.h>
#include <imgui_stdlib.h>
#include <FontAwesome5/IconsFontAwesome5.h>

PanelComponent::PanelComponent()
{
	particle_system_panel = new PanelParticleSystem();
}

PanelComponent::~PanelComponent()
{
	delete particle_system_panel;
}

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

		ImGui::Checkbox("Shadow caster", &mesh_renderer->shadow_caster);


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

		std::string skeleton_name = mesh_renderer->skeleton == nullptr ? "None (Skeleton)" : App->resources->resource_DB->GetEntry(mesh_renderer->skeleton->GetUUID())->resource_name;;
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

		ImGui::Checkbox("Is Raycastable", &mesh_renderer->is_raycastable);
	}
}

void PanelComponent::ShowComponentParticleSystem(ComponentParticleSystem* particle_system)
{
	particle_system_panel->Render(particle_system);
}

void PanelComponent::ShowComponentBillboard(ComponentBillboard* billboard)
{
	if (ImGui::CollapsingHeader(ICON_FA_SQUARE " Billboard", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (!ShowCommonComponentWindow(billboard))
		{
			return;
		}
		ImGui::Separator();

		ShowBillboardOptions(billboard);

	}
}

void PanelComponent::ShowBillboardOptions(ComponentBillboard* billboard)
{
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

	ImGui::ColorEdit4("Color", billboard->color);

	int alignment_type = static_cast<int>(billboard->alignment_type);
	if (ImGui::Combo("Billboard type", &alignment_type, "World\0View point\0Axial")) {
		switch (alignment_type)
		{
		case 0:
			billboard->ChangeBillboardType(ComponentBillboard::AlignmentType::WORLD);
			break;
		case 1:
			billboard->ChangeBillboardType(ComponentBillboard::AlignmentType::VIEW_POINT);
			break;
		case 2:
			billboard->ChangeBillboardType(ComponentBillboard::AlignmentType::AXIAL);
			break;
		}
	}

	ImGui::Checkbox("Spritesheet", &billboard->is_spritesheet);

	if (billboard->is_spritesheet) 
	{
		ImGui::DragInt("Columns", &billboard->num_sprisheet_columns);
		ImGui::DragInt("Rows", &billboard->num_sprisheet_rows);
		ImGui::DragInt("Animation Time", &billboard->animation_time, 10.f, 0);
		ImGui::Checkbox("Loop", &billboard->loop);

		if (ImGui::Button("Play"))
		{
			billboard->Play();
		}
	}

	ImGui::Separator();
	ImGui::Text("Custom");
	ImGui::DragFloat("Width:", &billboard->width, 0.2f, 0.f, 10.f);
	ImGui::DragFloat("Height:", &billboard->height, 0.2f, 0.f, 10.f);
	ImGui::DragFloat("Transparency:", &billboard->transparency, 0.1f, 0.f, 1.f);
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
		if (!ShowCommonComponentWindow(light))
		{
			return;
		}
		ImGui::Separator();

		if (ImGui::ColorEdit3("Color", light->light_color)) { light->modified_by_user = true; }

		App->actions->CheckClickForUndo(ModuleActions::UndoActionType::EDIT_COMPONENTLIGHT, light);

		if (ImGui::DragFloat("Intensity ", &light->light_intensity, 0.01f, 0.f, 100.f)) { light->modified_by_user = true; }

		App->actions->CheckClickForUndo(ModuleActions::UndoActionType::EDIT_COMPONENTLIGHT, light);

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
			ImGui::InputText("Active State", &animation->animation_controller->active_state->name, ImGuiInputTextFlags_ReadOnly);
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
			ImGui::InputText("Playing clip:", &playing_clip.clip->name, ImGuiInputTextFlags_ReadOnly);
			ImGui::Checkbox("Loop", &(playing_clip.clip->loop));
			ImGui::SliderFloat("Animation time", &playing_clip.current_time, 0, playing_clip.clip->animation_time);
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

void PanelComponent::ShowComponentImageWindow(ComponentImage* component_image)
{
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

		ImGui::ColorEdit3("Color", component_image->color.ptr());

		ImGui::Checkbox("Preserve Aspect Ratio", &component_image->preserve_aspect_ratio);

		if (ImGui::Button("Set Native Size"))
		{
			component_image->SetNativeSize();
		}
	}
}

void PanelComponent::ShowComponentSpriteMaskWindow(ComponentSpriteMask* component_mask)
{
	if (ImGui::CollapsingHeader(ICON_FA_THEATER_MASKS " Sprite Mask", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (!ShowCommonComponentWindow(component_mask))
		{
			return;
		}

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::Checkbox("Invert mask", &component_mask->inverted_mask);
		ImGui::Checkbox("Show sprite mask", &component_mask->render_mask);
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

		int horizontal_alignment = static_cast<int>(text->horizontal_alignment);
		if (ImGui::Combo("Horizontal Alignment", &horizontal_alignment, "Left\0Center\0Right"))
		{
			switch (horizontal_alignment)
			{
			case 0:
				text->SetHorizontalAlignment(ComponentText::HorizontalAlignment::LEFT);
				break;
			case 1:
				text->SetHorizontalAlignment(ComponentText::HorizontalAlignment::CENTER);
				break;
			case 2:
				text->SetHorizontalAlignment(ComponentText::HorizontalAlignment::RIGHT);
				break;
			}
		}

	}
}
void PanelComponent::ShowComponentButtonWindow(ComponentButton *button)
{
	if (ImGui::CollapsingHeader(ICON_FA_TOGGLE_ON " Button", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ShowCommonComponentWindow(button);
		bool clicked = button->IsClicked();
		ImGui::Checkbox("Clicked", &clicked);
	}
}

void PanelComponent::ShowComponentEventSystem(ComponentEventSystem* event_system)
{
	if (ImGui::CollapsingHeader(ICON_FA_BULLHORN " Event System", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ShowCommonComponentWindow(event_system);
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

		ImGui::Separator();

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

		ImGui::Separator();

		sprintf_s(tmp_string, "%s Transform 2D", ICON_FA_RULER_COMBINED);
		if (ImGui::Selectable(tmp_string))
		{
			App->editor->selected_game_object->SetTransform2DStatus(true);
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

		sprintf_s(tmp_string, "%s Sprite Mask", ICON_FA_THEATER_MASKS);
		if (ImGui::Selectable(tmp_string))
		{
			component = App->editor->selected_game_object->CreateComponent(Component::ComponentType::UI_SPRITE_MASK);
		}

		sprintf_s(tmp_string, "%s UI Button", ICON_FA_TOGGLE_ON);
		if (ImGui::Selectable(tmp_string))
		{
			component = App->editor->selected_game_object->CreateComponent(Component::ComponentType::UI_BUTTON);
		}

		sprintf_s(tmp_string, "%s Event System", ICON_FA_BULLHORN);
		if (ImGui::Selectable(tmp_string))
		{
			component = App->editor->selected_game_object->CreateComponent(Component::ComponentType::EVENT_SYSTEM);
		}
		ImGui::Separator();

		sprintf_s(tmp_string, "%s Audio Source", ICON_FA_VOLUME_UP);
		if (ImGui::Selectable(tmp_string))
		{
			component = App->editor->selected_game_object->CreateComponent(Component::ComponentType::AUDIO_SOURCE);

		}

		sprintf_s(tmp_string, "%s Audio Listener", ICON_FA_HEADPHONES);
		if (ImGui::Selectable(tmp_string))
		{
			component = App->editor->selected_game_object->CreateComponent(Component::ComponentType::AUDIO_LISTENER);

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
			App->editor->popups->create_script_popup_shown = true;
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
	if (ImGui::Button("Copy"))
	{
		App->actions->SetCopyComponent(component);
	}
	if (component->type != Component::ComponentType::MESH_RENDERER) 
	{
		if (ImGui::Button("Paste component as new"))
		{
			App->actions->PasteComponent(component);
		}
	}
	if (ImGui::Button("Paste component values"))
	{
		App->actions->PasteComponentValues(component);
	}

	return true;
}

bool PanelComponent::ShowCommonColliderWindow(ComponentCollider* collider)
{
	if (ImGui::Checkbox("Active", &collider->active))
	{
		//UndoRedo
		collider->SwitchPhysics();
		App->actions->action_component = collider;
		App->actions->AddUndoAction(ModuleActions::UndoActionType::ENABLE_DISABLE_COMPONENT);
	}
	ImGui::SameLine();
	if (ImGui::Button("Delete"))
	{
		App->actions->DeleteComponentUndo(collider);
		return false;
	}
	if (ImGui::Button("Copy")) 
	{
		App->actions->SetCopyComponent(collider);
	}
	if (ImGui::Button("Paste component as new"))
	{
		App->actions->PasteComponent(collider);
	}
	if (ImGui::Button("Paste component values"))
	{
		App->actions->PasteComponentValues(collider);
	}
	ImGui::Separator();

	if (ImGui::DragFloat("Mass", &collider->mass, 1.0F, 0.1F, 1000.F))
	{
		collider->SetMass(collider->mass);
	}
	if (ImGui::DragFloat("Friction", &collider->friction, 0.1F, 0.1F, 1.F)) {
		collider->UpdateFriction();
	}
	if (ImGui::DragFloat("Rolling friction", &collider->rolling_friction, 1.0F, 0.1F, 2000.F))
	{
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
	if (ImGui::Checkbox("Collision Detection", &collider->detect_collision))
	{
		collider->SetCollisionDetection();
	}
	if (ImGui::Checkbox("Active Physics", &collider->active_physics))
	{
		collider->SwitchPhysics();
	}
	ImGui::Text("Freeze Axis Rotation");
	if (ImGui::Checkbox("X Axis", &collider->freeze_rotation_x))
	{
		collider->SetRotationAxis();
	}
	if (ImGui::Checkbox("Y Axis", &collider->freeze_rotation_y))
	{
		collider->SetRotationAxis();
	}
	if (ImGui::Checkbox("Z Axis", &collider->freeze_rotation_z))
	{
		collider->SetRotationAxis();
	}
	if (ImGui::DragFloat3("Center", collider->center.ptr(), 0.01F))
	{
		collider->SetColliderCenter(collider->center);
	}
	return true;
}


void PanelComponent::ShowComponentBoxColliderWindow(ComponentBoxCollider* box_collider)
{
	if (ImGui::CollapsingHeader(ICON_FA_BOX " Box Collider", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ShowCommonColliderWindow(box_collider))
		{
			if (ImGui::DragFloat3("Scale", box_collider->scale.ptr(), 0.01F, 0.1F, 10.0F))
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
			if (ImGui::DragFloat("Radius", &capsule_collider->scale.x, 0.01F, 0.1F, 10.0F))
			{
				capsule_collider->Scale();
			}
			if (ImGui::DragFloat("Height", &capsule_collider->scale.y, 0.01F, 0.1F, 10.0F))
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
			if (ImGui::DragFloat("Radius", &sphere_collider->scale.x, 0.01F, 0.01F, 10.0F))
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
			if (ImGui::DragFloat("Radius", &cylinder_collider->scale.x, 0.01F, 0.1F, 10.0F))
			{
				cylinder_collider->Scale();
			}
			if (ImGui::DragFloat("Height", &cylinder_collider->scale.y, 0.01F, 0.1F, 10.0F))
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
			if (ImGui::DragFloat3("Scale", mesh_collider->scale.ptr(), 0.01F))
			{
				mesh_collider->Scale();
			}
		}
	}
}

void PanelComponent::ShowComponentAudioListenerWindow(ComponentAudioListener* component_audio_listener)
{
	if (ImGui::CollapsingHeader(ICON_FA_HEADPHONES " Audio Listener", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (!ShowCommonComponentWindow(component_audio_listener))
		{
			return;
		}
		ImGui::Separator();

		//Add More Stuff here 

	}
	
}

void PanelComponent::ShowComponentAudioSourceWindow(ComponentAudioSource* component_audio_source)
{
	if (ImGui::CollapsingHeader(ICON_FA_VOLUME_UP " Audio Source", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (!ShowCommonComponentWindow(component_audio_source))
		{
			return;
		}
		ImGui::Separator();

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
		selected_resource = ImGui::ResourceDropper<SoundBank>();
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
