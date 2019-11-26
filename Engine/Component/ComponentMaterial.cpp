#include "ComponentMaterial.h"
#include "Texture.h"

#include "imgui.h"
#include "IconsFontAwesome5.h"

ComponentMaterial::~ComponentMaterial()
{
	delete texture;
}

void ComponentMaterial::Enable()
{

}

void ComponentMaterial::Disable()
{

}

void ComponentMaterial::Update()
{

}

Component::ComponentType ComponentMaterial::GetType() const
{
	return Component::ComponentType::MATERIAL;
}

void ComponentMaterial::ShowComponentWindow()
{
	if (ImGui::CollapsingHeader(ICON_FA_IMAGE" Material"), ImGuiTreeNodeFlags_DefaultOpen)
	{
		char tmp_string[256];
		int window_width = ImGui::GetWindowWidth();

		ImGui::Text("Texture path:");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), texture->texture_path.c_str());

		sprintf_s(tmp_string, "%dx%d px", texture->width, texture->height);
		ImGui::Text("Texture size:");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), tmp_string);

		ImGui::Text("Mipmap:");
		ImGui::SameLine();
		if (texture->IsMipMapped())
		{
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Yes");
		}
		else
		{
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "No");
		}

		ImGui::Text("Wrap S:");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), &texture->GetWrapS_C_Str()[0]);

		ImGui::Text("Wrap T:");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), &texture->GetWrapT_C_Str()[0]);

		ImGui::Text("Min filter:");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), &texture->GetMinFilter_C_Str()[0]);

		ImGui::Text("Mag filter:");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), &texture->GetMagFilter_C_Str()[0]);

		ImGui::Spacing();

		ImGui::SetCursorPosX(window_width*0.25f);
		ImGui::Image((void*)(intptr_t)texture->opengl_texture, ImVec2(window_width * 0.5f, window_width * 0.5f), ImVec2(0, 1), ImVec2(1, 0));

	}
}