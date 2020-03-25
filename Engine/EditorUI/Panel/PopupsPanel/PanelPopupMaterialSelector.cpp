#include "PanelPopupMaterialSelector.h"

#include "Component/ComponentMeshRenderer.h"
#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleEditor.h"
#include "Module/ModuleResourceManager.h"
#include "Module/ModuleTexture.h"

#include <imgui.h>

PanelPopupMaterialSelector::PanelPopupMaterialSelector()
{
	opened = false;
	enabled = true;
	window_name = "Popups Material Selector";
}

void PanelPopupMaterialSelector::Render()
{
	if (show_material_selector_popup)
	{
		show_material_selector_popup = false;
		opened = true;

		material_metafiles = std::vector<Metafile*>();
		App->resources->resource_DB->GetEntriesOfType(material_metafiles, ResourceType::MATERIAL);
	}

	if (!opened)
	{
		return;
	}

	ImGui::SetNextWindowSize(ImVec2(material_icon_size * 4.5, material_icon_size * 2 * 1.1f));

	if (ImGui::Begin("Select Material", &opened))
	{
		hovered = ImGui::IsWindowHovered();

		child_window_focused = false;

		ImVec2 available_region = ImGui::GetContentRegionAvail();
		int files_per_line = available_region.x / material_icon_size;

		int current_line = 0;
		int current_file_in_line = 0;

		for (auto& material_metafile : material_metafiles)
		{
			ImGui::PushID(current_line * files_per_line + current_file_in_line);
			ShowMaterialIcon(material_metafile);
			ImGui::PopID();

			++current_file_in_line;
			if (current_file_in_line == files_per_line)
			{
				current_file_in_line = 0;
				++current_line;
			}
			else
			{
				ImGui::SameLine();
			}

		}

		if (child_window_focused)
		{
			ImGui::SetWindowFocus();
		}
		focused = ImGui::IsWindowFocused();

		if (!focused)
		{
			opened = false;
		}
	}
	ImGui::End();
}

void PanelPopupMaterialSelector::ShowMaterialIcon(Metafile* material_metafile)
{
	Path* material_imported_file_path = App->filesystem->GetPath(material_metafile->imported_file_path);
	std::string filename = std::string(material_imported_file_path->file_name_no_extension);
	if (ImGui::BeginChild(filename.c_str(), ImVec2(material_icon_size, material_icon_size), selected_material == material_metafile, ImGuiWindowFlags_NoDecoration))
	{
		ProcessMaterialMouseInput(material_metafile);

		ImGui::SetCursorPosX((ImGui::GetWindowWidth() - 0.75 * material_icon_size) * 0.5f);
		ImGui::Image((void *)App->texture->whitefall_texture_id, ImVec2(0.75*material_icon_size, 0.75 * material_icon_size)); // TODO: Substitute this with resouce thumbnail
		ImGui::Spacing();

		float text_width = ImGui::CalcTextSize(filename.c_str()).x;
		if (text_width < material_icon_size)
		{
			ImGui::SetCursorPosX((ImGui::GetWindowWidth() - text_width) * 0.5f);
			ImGui::Text(filename.c_str());
		}
		else
		{
			int character_width = text_width / filename.length();
			int string_position_wrap = material_icon_size / character_width - 5;
			assert(string_position_wrap < filename.length());
			std::string wrapped_filename = filename.substr(0, string_position_wrap) + "...";
			ImGui::Text(wrapped_filename.c_str());
		}
	}
	ImGui::EndChild();
}

void PanelPopupMaterialSelector::ProcessMaterialMouseInput(Metafile* file)
{
	if (ImGui::IsWindowHovered())
	{
		if (ImGui::IsMouseClicked(0))
		{
			if (selected_material != file)
			{
				selected_material = file;
				ChangeSelectedObjectMaterial();
			}
		}

		if (ImGui::IsMouseDoubleClicked(0))
		{
			opened = false;
		}
	}

	if (ImGui::IsWindowFocused())
	{
		child_window_focused = true;
	}
}

void PanelPopupMaterialSelector::ChangeSelectedObjectMaterial() const
{
	GameObject* selected_gameobject = App->editor->selected_game_object;
	Component* selected_gameobject_mesh_renderer_component = selected_gameobject->GetComponent(Component::ComponentType::MESH_RENDERER);
	assert(selected_gameobject_mesh_renderer_component != nullptr);
	ComponentMeshRenderer* selected_gameobject_mesh_renderer = static_cast<ComponentMeshRenderer*>(selected_gameobject_mesh_renderer_component);

	std::shared_ptr<Material> new_material = std::static_pointer_cast<Material>(App->resources->Load(selected_material->uuid));
	selected_gameobject_mesh_renderer->SetMaterial(new_material);
	selected_gameobject_mesh_renderer->modified_by_user = true;
}