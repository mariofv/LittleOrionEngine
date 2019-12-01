#include "ComponentsUI.h"
#include "Component/ComponentMaterial.h"
#include "Component/ComponentMesh.h"
#include "Component/ComponentTransform.h"
#include "Texture.h"

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
		if (ImGui::DragFloat3("Rotation", &transform->rotation[0], 0.1f, -180.f, 180.f))
		{
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

		ImGui::DragInt("# Triangles", &mesh->num_triangles, NULL, NULL, NULL);
		ImGui::DragInt("# Vertices", &mesh->num_vertices, NULL, NULL, NULL);
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

		ImGui::Text("Texture path:");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), material->texture->texture_path.c_str());

		sprintf_s(tmp_string, "%dx%d px", material->texture->width, material->texture->height);
		ImGui::Text("Texture size:");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), tmp_string);

		ImGui::Text("Mipmap:");
		ImGui::SameLine();
		if (material->texture->IsMipMapped())
		{
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Yes");
		}
		else
		{
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "No");
		}

		ImGui::Text("Wrap S:");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), &material->texture->GetWrapS_C_Str()[0]);

		ImGui::Text("Wrap T:");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), &material->texture->GetWrapT_C_Str()[0]);

		ImGui::Text("Min filter:");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), &material->texture->GetMinFilter_C_Str()[0]);

		ImGui::Text("Mag filter:");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), &material->texture->GetMagFilter_C_Str()[0]);

		ImGui::Spacing();

		ImGui::SetCursorPosX(window_width*0.25f);
		ImGui::Image((void*)(intptr_t)material->texture->opengl_texture, ImVec2(window_width * 0.5f, window_width * 0.5f), ImVec2(0, 1), ImVec2(1, 0));

		ImGui::Checkbox("Checker Texture", &material->show_checkerboard_texture);
	}
}