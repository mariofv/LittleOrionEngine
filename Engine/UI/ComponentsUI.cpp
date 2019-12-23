#include "ComponentsUI.h"
#include "Component/ComponentCamera.h"
#include "Component/ComponentMaterial.h"
#include "Component/ComponentMesh.h"
#include "Component/ComponentTransform.h"
#include "Texture.h"
#include "Utils.h"

#include <imgui.h>
#include <FontAwesome5/IconsFontAwesome5.h>

void ComponentsUI::ShowComponentTransformWindow(ComponentTransform *transform)
{
	if (ImGui::CollapsingHeader(ICON_FA_RULER_COMBINED " Transform", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::DragFloat3("Translation", &transform->translation[0], 0.01f))
		{
			transform->GenerateModelMatrix();
		}

		float3 temp_rotation = Utils::GenerateDegFloat3FromQuat(transform->rotation);
		if (ImGui::DragFloat3("Rotation", &temp_rotation[0], 0.1f, -180.f, 180.f))
		{
			transform->rotation = Utils::GenerateQuatFromDegFloat3(temp_rotation);
			transform->GenerateModelMatrix();
		}
		if (ImGui::DragFloat3("Scale", &transform->scale[0], 0.01f))
		{
			transform->GenerateModelMatrix();
		}
	}
}

void ComponentsUI::ShowComponentMeshWindow(ComponentMesh *mesh)
{
	if (ImGui::CollapsingHeader(ICON_FA_SHAPES " Mesh", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Checkbox("Active", &mesh->active);
		ImGui::Separator();


		char tmp_string[16];
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Triangles");
		ImGui::SameLine();
		sprintf(tmp_string, "%d", mesh->mesh_to_render->num_triangles);
		ImGui::Button(tmp_string);

		ImGui::AlignTextToFramePadding();
		ImGui::Text("Vertices");
		ImGui::SameLine();
		sprintf(tmp_string, "%d", mesh->mesh_to_render->num_vertices);
		ImGui::Button(tmp_string);
	}
}

void ComponentsUI::ShowComponentMaterialWindow(ComponentMaterial *material)
{
	if (ImGui::CollapsingHeader(ICON_FA_IMAGE " Material", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Checkbox("Active", &material->active);
		ImGui::Separator();


		char tmp_string[256];
		int window_width = ImGui::GetWindowWidth();
		ImGui::Image((void*)(intptr_t)material->texture_diffuse->opengl_texture, ImVec2(window_width * 0.2f, window_width * 0.2f), ImVec2(0, 1), ImVec2(1, 0));
		ImGui::SameLine();
		ImGui::BeginGroup();
		ImGui::Text("Texture:");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), material->texture_diffuse->texture_path.c_str());
		sprintf_s(tmp_string, "(%dx%d px)", material->texture_diffuse->width, material->texture_diffuse->height);
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), tmp_string);

		bool mipmap = material->texture_diffuse->IsMipMapped();
		ImGui::Checkbox("Mipmap", &mipmap);
		ImGui::Spacing();

		ImGui::Checkbox("Checker Texture", &material->show_checkerboard_texture);
		ImGui::EndGroup();
	}
}

void ComponentsUI::ShowComponentCameraWindow(ComponentCamera *camera)
{
	if (ImGui::CollapsingHeader(ICON_FA_VIDEO " Camera", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::InputFloat3("Front", &camera->camera_frustum.front[0], 3, ImGuiInputTextFlags_ReadOnly);
		ImGui::InputFloat3("Up", &camera->camera_frustum.up[0], 3, ImGuiInputTextFlags_ReadOnly);

		ImGui::Separator();

		ImGui::SliderFloat("Mov Speed", &camera->camera_movement_speed, camera->CAMERA_MINIMUN_MOVEMENT_SPEED, camera->CAMERA_MAXIMUN_MOVEMENT_SPEED);
		ImGui::SliderFloat("Rot Speed", &camera->camera_rotation_speed, camera->CAMERA_MINIMUN_MOVEMENT_SPEED, camera->CAMERA_MAXIMUN_MOVEMENT_SPEED);
		ImGui::SliderFloat("Zoom Speed", &camera->camera_zooming_speed, camera->CAMERA_MINIMUN_MOVEMENT_SPEED, camera->CAMERA_MAXIMUN_MOVEMENT_SPEED);

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
				camera->camera_clear_mode = ComponentCamera::ClearMode::COLOR;
				break;
			case 1:
				camera->camera_clear_mode = ComponentCamera::ClearMode::SKYBOX;
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
	}
}