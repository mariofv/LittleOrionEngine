#include "ComponentsUI.h"
#include "Component/ComponentCamera.h"
#include "Component/ComponentMaterial.h"
#include "Component/ComponentMesh.h"
#include "Component/ComponentTransform.h"
#include "Component/ComponentLight.h"

#include "Application.h"
#include "GameObject.h"
#include "Module/ModuleFileSystem.h"
#include "Module/ModuleTexture.h"
#include "Module/ModuleProgram.h"
#include "Module/ModuleEditor.h"

#include "Actions/EditorActionTranslate.h"
#include "Actions/EditorActionRotation.h"
#include "Actions/EditorActionScale.h"
#include "Actions/EditorAction.h"

#include "Utils.h"

#include <imgui.h>
#include <FontAwesome5/IconsFontAwesome5.h>

void ComponentsUI::ShowComponentTransformWindow(ComponentTransform *transform)
{
	if (ImGui::CollapsingHeader(ICON_FA_RULER_COMBINED " Transform", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (App->editor->clicked && !ImGui::IsMouseDragging())
		{
			App->editor->AddUndoAction(App->editor->type_of_action);
			App->editor->clicked = false;
		}

		//TODO: look for boolean imgui::dragfloat3

		if (ImGui::DragFloat3("Translation", transform->translation.ptr(), 0.01f))
		{
			transform->OnTransformChange();

			if (!App->editor->clicked && ImGui::IsMouseDragging())
			{
				App->editor->clicked = true;
				App->editor->previous_transform = transform->GetTranslation();
				App->editor->type_of_action = 0;
			}

		}

		if (ImGui::DragFloat3("Rotation", transform->rotation_degrees.ptr(), 0.1f, -180.f, 180.f))
		{
			transform->rotation = Utils::GenerateQuatFromDegFloat3(transform->rotation_degrees);
			transform->rotation_radians = Utils::Float3DegToRad(transform->rotation_degrees);
			transform->OnTransformChange();

			if (!App->editor->clicked && ImGui::IsMouseDragging())
			{
				App->editor->clicked = true;
				App->editor->previous_transform = transform->GetRotationRadiants();
				App->editor->type_of_action = 1;
			}

		}
		if (ImGui::DragFloat3("Scale", transform->scale.ptr(), 0.01f))
		{
			transform->OnTransformChange();

			if (!App->editor->clicked && ImGui::IsMouseDragging())
			{
				App->editor->clicked = true;
				App->editor->previous_transform = transform->GetScale();
				App->editor->type_of_action = 2;
			}

		}
	}
}

void ComponentsUI::ShowComponentMeshWindow(ComponentMesh *mesh)
{
	if (ImGui::CollapsingHeader(ICON_FA_SHAPES " Mesh", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Checkbox("Active", &mesh->active);
		ImGui::SameLine();
		if (ImGui::Button("Delete"))
		{
			//mesh->owner->RemoveComponent(mesh);
			App->editor->DeleteComponentUndo(mesh);
			return;
		}
		ImGui::Separator();


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

		if (ImGui::BeginCombo("Shader", mesh->shader_program.c_str()))
		{
			for (auto & program : App->program->names)
			{
				bool is_selected = (mesh->shader_program == program);
				if (ImGui::Selectable(program, is_selected))
				{
					mesh->shader_program = program;
					if (is_selected)
						ImGui::SetItemDefaultFocus();  
				}

			}
			ImGui::EndCombo();
		}
	}
}

void ComponentsUI::ShowComponentMaterialWindow(ComponentMaterial *material)
{
	if (ImGui::CollapsingHeader(ICON_FA_IMAGE " Material", ImGuiTreeNodeFlags_DefaultOpen))
	{
		float window_width = ImGui::GetWindowWidth();
		for (size_t i = 0; i < material->textures.size(); ++i)
		{
			Texture::TextureType type = static_cast<Texture::TextureType>(i);
			if (ImGui::CollapsingHeader(GetTypeName(type).c_str(), ImGuiTreeNodeFlags_DefaultOpen))
			{
				if (material->textures[i].get() != nullptr) {
					ImGui::PushID(i);
					char tmp_string[256];
					std::shared_ptr<Texture> & texture = material->textures[i];
					ImGui::Image((void*)(intptr_t)texture->opengl_texture, ImVec2(window_width * 0.2f, window_width * 0.2f), ImVec2(0, 1), ImVec2(1, 0));
					DropTarget(material, type);
					ImGui::SameLine();
					ImGui::BeginGroup();
					ImGui::Text("Texture:");
					ImGui::SameLine();
					ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), texture->texture_path.c_str());
					sprintf_s(tmp_string, "(%dx%d px)", texture->width, texture->height);
					ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), tmp_string);

					bool mipmap = texture->IsMipMapped();
					ImGui::Checkbox("Mipmap", &mipmap);
					ImGui::SameLine();
					ImGui::Checkbox("Checker Texture", &material->show_checkerboard_texture);
					ImGui::Spacing();

					if (ImGui::Button(ICON_FA_TIMES) )
					{
						material->RemoveMaterialTexture(i);
					}
					ImGui::SameLine(); ImGui::Text("Remove Texture");
					ImGui::EndGroup();
					ImGui::PopID();
				}
				else
				{
					ImGui::Image((void*)0, ImVec2(window_width * 0.2f, window_width * 0.2f), ImVec2(0, 1), ImVec2(1, 0), ImVec4(1.f,1.f,1.f,1.f), ImVec4(1.f, 1.f, 1.f, 1.f));
					DropTarget(material, type);
				}
				if (type == Texture::TextureType::DIFUSSE)
				{
					ImGui::ColorEdit3("Diffuse Color", material->diffuse_color);
					ImGui::SliderFloat("k diffuse", &material->k_diffuse, 0, 1);
				}
				if (type == Texture::TextureType::EMISSIVE)
				{
					ImGui::ColorEdit3("Emissive Color", material->emissive_color);
				}
				if (type == Texture::TextureType::OCLUSION)
				{
					ImGui::SliderFloat("k ambient", &material->k_ambient, 0, 1);
				}
				if (type == Texture::TextureType::SPECULAR)
				{
					ImGui::ColorEdit3("Specular Color", material->specular_color);
					ImGui::SliderFloat("k specular", &material->k_specular, 0, 1);
					ImGui::SliderFloat("Shininess", &material->shininess, 0, 1);
				}

				ImGui::Separator();
			}
		}
	}
}
void ComponentsUI::DropTarget(ComponentMaterial *material, Texture::TextureType type)
{
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_File"))
		{
			assert(payload->DataSize == sizeof(File*));
			File *incoming_file = *(File**)payload->Data;
			if (incoming_file->file_type == FileType::TEXTURE)
			{
				//UndoRedo
				App->editor->type_texture = type;
				App->editor->action_component = material;
				App->editor->AddUndoAction(8);

				material->SetMaterialTexture(type, App->texture->LoadTexture(incoming_file->file_path.c_str()));
			}
		}
		ImGui::EndDragDropTarget();
	}
}

std::string ComponentsUI::GetTypeName(Texture::TextureType type)
{
	switch (type)
	{
	case Texture::TextureType::DIFUSSE:
		return "Difusse";
		break;
	case Texture::TextureType::SPECULAR:
		return "Specular";
		break;
	case Texture::TextureType::EMISSIVE:
		return "Emissive";
		break;
	case Texture::TextureType::OCLUSION:
		return "Oclusion";
		break;
	default:
		return "";
	}
}
void ComponentsUI::ShowComponentCameraWindow(ComponentCamera *camera)
{
	if (ImGui::CollapsingHeader(ICON_FA_VIDEO " Camera", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Checkbox("Active", &camera->active);
		ImGui::SameLine();
		if (ImGui::Button("Delete"))
		{
			//camera->owner->RemoveComponent(camera);
			App->editor->DeleteComponentUndo(camera);
			return;
		}
		ImGui::Separator();

		ImGui::InputFloat3("Front", &camera->camera_frustum.front[0], 3, ImGuiInputTextFlags_ReadOnly);
		ImGui::InputFloat3("Up", &camera->camera_frustum.up[0], 3, ImGuiInputTextFlags_ReadOnly);

		ImGui::Separator();

		ImGui::SliderFloat("Mov Speed", &camera->camera_movement_speed, camera->CAMERA_MINIMUN_MOVEMENT_SPEED, camera->CAMERA_MAXIMUN_MOVEMENT_SPEED);

		if (ImGui::SliderFloat("FOV", &camera->camera_frustum.horizontalFov, 0, 2 * 3.14f))
		{
			camera->SetFOV(camera->camera_frustum.horizontalFov);
		}

		if (ImGui::SliderFloat("Aspect Ratio", &camera->aspect_ratio, 0, 10))
		{
			camera->SetAspectRatio(camera->aspect_ratio);
		}

		if (ImGui::SliderFloat("Near plane", &camera->camera_frustum.nearPlaneDistance, 1, camera->camera_frustum.farPlaneDistance + 1))
		{
			camera->SetNearDistance(camera->camera_frustum.nearPlaneDistance);
		}

		if (ImGui::SliderFloat("Far plane", &camera->camera_frustum.farPlaneDistance, camera->camera_frustum.nearPlaneDistance + 1, camera->camera_frustum.nearPlaneDistance + 1000))
		{
			camera->SetFarDistance(camera->camera_frustum.farPlaneDistance);
		}

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

		if (ImGui::SliderFloat("Orthographic Size", &camera->camera_frustum.orthographicHeight, 0, 100))
		{
			camera->SetOrthographicSize(float2(camera->camera_frustum.orthographicHeight * camera->aspect_ratio, camera->camera_frustum.orthographicHeight));
		}

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
	}
}


void ComponentsUI::ShowComponentLightWindow(ComponentLight *light)
{
	if (ImGui::CollapsingHeader(ICON_FA_LIGHTBULB " Light", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Checkbox("Active", &light->active);
		ImGui::SameLine();
		if (ImGui::Button("Delete"))
		{
			//light->owner->RemoveComponent(light);
			App->editor->DeleteComponentUndo(light);

			return;
		}
		ImGui::Separator();

		if(ImGui::ColorEdit3("Color", light->light_color))
		{
			if(!App->editor->clicked_light)
			{
				//UndoRedo
				App->editor->previous_light_color[0] = light->light_color[0];
				App->editor->previous_light_color[1] = light->light_color[1];
				App->editor->previous_light_color[2] = light->light_color[2];
				App->editor->previous_light_intensity = light->light_intensity;

				App->editor->clicked_light = true;
			}
		}
		else if (ImGui::SliderFloat("Intensity ", &light->light_intensity, 0.f, 1.f))
		{
			if (!App->editor->clicked_light)
			{
				//UndoRedo
				App->editor->previous_light_color[0] = light->light_color[0];
				App->editor->previous_light_color[1] = light->light_color[1];
				App->editor->previous_light_color[2] = light->light_color[2];
				App->editor->previous_light_intensity = light->light_intensity;


				App->editor->clicked_light = true;
			}
		}
		if(App->editor->clicked_light && !ImGui::IsMouseDown(0))
		{
			//Push new action
			App->editor->action_component = light;
			App->editor->AddUndoAction(7);

			App->editor->clicked_light = false;
		}
	}
}

