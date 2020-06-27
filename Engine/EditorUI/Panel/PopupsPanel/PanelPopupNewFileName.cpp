#include "PanelPopupNewFileName.h"

#include "Main/Application.h"
#include "Module/ModuleEditor.h"

#include <imgui_internal.h>
#include <imgui_stdlib.h>
void PanelPopupNewFileName::Render()
{

	if (show_new_filename_popup)
	{
		show_new_filename_popup = false;
		opened = true;
		ImGui::OpenPopup("Choose name");
	}

	if (ImGui::BeginPopupModal("Choose name", NULL))
	{
		ImGui::PushItemWidth(-1);
		ImGui::InputText("", &new_filename);
		ImGui::PopItemWidth();
		bool disabled = new_filename.empty();
		if (disabled)
		{
			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
		}
		if (ImGui::Button("ok", ImVec2(150, 0)))
		{
			apply_new_name(new_filename);
			pending_to_be_closed = true;
			new_filename = {};
			apply_new_name = {};
		}
		if (disabled)
		{
			ImGui::PopItemFlag();
			ImGui::PopStyleVar();
		}
		ImGui::SameLine();
		if (ImGui::Button("cancel"))
		{
			ImGui::CloseCurrentPopup();
		}

		if (pending_to_be_closed)
		{
			pending_to_be_closed = false;
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}
