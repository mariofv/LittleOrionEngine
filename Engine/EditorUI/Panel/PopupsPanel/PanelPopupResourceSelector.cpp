#include "PanelPopupResourceSelector.h"

#include "Component/ComponentMeshRenderer.h"
#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleEditor.h"
#include "Module/ModuleResourceManager.h"
#include "Module/ModuleTexture.h"

#include <imgui.h>

PanelPopupResourceSelector::PanelPopupResourceSelector()
{
	opened = false;
	enabled = true;
	window_name = "Popups Resource Selector";
}

void PanelPopupResourceSelector::ShowPanel(unsigned int element_id , ResourceType resource_type)
{
	show_resource_selector_popup = true;
	this->element_id = element_id;
	this->resource_type = resource_type;
}

void PanelPopupResourceSelector::ClosePanel()
{
	opened = false;
	selected_resource = 0;
	this->element_id = 0;
	this->resource_type = ResourceType::UNKNOWN;
}

uint32_t PanelPopupResourceSelector::GetSelectedResource(unsigned int element_id)
{
	if (this->element_id != element_id)
	{
		return 0;
	}
	else
	{
		uint32_t tmp = selected_resource;
		selected_resource = 0;
		return tmp;
	}
}

void PanelPopupResourceSelector::Render()
{
	if (show_resource_selector_popup)
	{
		show_resource_selector_popup = false;
		opened = true;

		resource_metafiles = std::vector<Metafile*>();
		App->resources->resource_DB->GetEntriesOfType(resource_metafiles, resource_type);
		resource_name = Resource::GetResourceTypeName(resource_type);
	}

	if (!opened)
	{
		return;
	}

	ImGui::SetNextWindowSize(ImVec2(resource_icon_size * 4.5f, resource_icon_size * 2 * 1.1f));

	if (ImGui::Begin(("Select " + resource_name).c_str(), &opened))
	{
		hovered = ImGui::IsWindowHovered();

		child_window_focused = false;

		ImVec2 available_region = ImGui::GetContentRegionAvail();
		int files_per_line = static_cast<int>(available_region.x / resource_icon_size);

		int current_line = 0;
		int current_file_in_line = 0;

		for (auto& resource_metafile : resource_metafiles)
		{
			ImGui::PushID(current_line * files_per_line + current_file_in_line);
			ShowResourceIcon(resource_metafile);
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
			ClosePanel();
		}
	}
	ImGui::End();
}

void PanelPopupResourceSelector::ShowResourceIcon(Metafile* resource_metafile)
{
	Path* resource_imported_file_path = App->filesystem->GetPath(resource_metafile->imported_file_path);
	std::string filename = resource_imported_file_path->GetFilenameWithoutExtension();
	if (ImGui::BeginChild(filename.c_str(), ImVec2(resource_icon_size, resource_icon_size), selected_resource_metafile == resource_metafile, ImGuiWindowFlags_NoDecoration))
	{
		ProcessMouseInput(resource_metafile);

		ImGui::SetCursorPosX((ImGui::GetWindowWidth() - 0.75f * resource_icon_size) * 0.5f);
		ImGui::Image((void *)App->texture->whitefall_texture_id, ImVec2(0.75f*resource_icon_size, 0.75f * resource_icon_size)); // TODO: Substitute this with resouce thumbnail
		ImGui::Spacing();

		float text_width = ImGui::CalcTextSize(filename.c_str()).x;
		if (text_width < resource_icon_size)
		{
			ImGui::SetCursorPosX((ImGui::GetWindowWidth() - text_width) * 0.5f);
			ImGui::Text(filename.c_str());
		}
		else
		{
			float character_width = text_width / filename.length();
			int string_position_wrap = static_cast<int>(resource_icon_size / character_width - 5);
			assert((size_t)string_position_wrap < filename.length());
			std::string wrapped_filename = filename.substr(0, string_position_wrap) + "...";
			ImGui::Text(wrapped_filename.c_str());
		}
	}
	ImGui::EndChild();
}

void PanelPopupResourceSelector::ProcessMouseInput(Metafile* file)
{
	if (ImGui::IsWindowHovered())
	{
		if (ImGui::IsMouseClicked(0))
		{
			if (selected_resource_metafile != file)
			{
				selected_resource_metafile = file;
				SelectResource();
			}
		}

		if (ImGui::IsMouseDoubleClicked(0))
		{
			ClosePanel();
		}
	}

	if (ImGui::IsWindowFocused())
	{
		child_window_focused = true;
	}
}

void PanelPopupResourceSelector::SelectResource()
{
	selected_resource = selected_resource_metafile->uuid;
}