#include "PanelPopupSceneManagement.h"

#include "Main/Application.h"
#include "Module/ModuleEditor.h"
#include "Module/ModuleFileSystem.h"
#include "Module/ModuleScene.h"

#include <algorithm>    // std::find
#include <Brofiler/Brofiler.h>
#include <imgui_internal.h>
#include <imgui_stdlib.h>


PanelPopupSceneManagement::PanelPopupSceneManagement()
{
	opened = false;
	enabled = true;

	window_name = "Popups Scene Management";
}

void PanelPopupSceneManagement::Render()
{
	if (popup_shown)
	{
		popup_shown = false;
		opened = true;

		selected_path = -1;
		selected_file_name = std::string();

		has_selected = false;

		current_path = App->filesystem->assets_folder_path;
		path_stack.clear();
		path_stack.push_back(current_path);

		ImGui::OpenPopup("Load Scene");
	}

	ImGui::SetNextWindowSize(ImVec2(700,450), ImGuiCond_Once);
	if (ImGui::BeginPopupModal("Load Scene", NULL))
	{
		assert(path_stack.back() == current_path);
		RenderAccessPath();
		RenderCurrentFolderContents();
		
		ImGui::PushItemWidth(-1);
		ImGui::InputText("", &selected_file_name);
		ImGui::PopItemWidth();

		if (ImGui::Button("ok") && selected_file_name != "")
		{
			if (App->filesystem->Exists(GetNormalizedPath()))
			{
				ImGui::OpenPopup("Confirmation");
			}
			else
			{
				SetPopupSelection();
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("cancel"))
		{
			ImGui::CloseCurrentPopup();
		}

		ConfirmationPopup();

		ImGui::EndPopup();
	}

	if (HasSelected())
	{
		APP_LOG_INFO("Loading %s scene.", GetSelected())
		
		App->editor->OpenScene(GetSelected());
		App->editor->current_scene_path = GetSelected();
	}
}

void PanelPopupSceneManagement::RenderAccessPath()
{
	for (size_t i = 0; i < path_stack.size(); ++i)
	{
		if (i != 0)
		{
			ImGui::SameLine();
		}

		Path* path = path_stack[i];
		if (ImGui::Button(path->GetFilename().c_str()))
		{
			PopCurrentPath(path);
			break;
		}
	}
}

void PanelPopupSceneManagement::RenderCurrentFolderContents()
{
	float reserved_space = GetFrameHeightWithSpacing() * 2;
	ImGui::BeginChild("###current_directory_browser", ImVec2(0, -reserved_space), true);

	if (current_path != App->filesystem->assets_folder_path)
	{
		if (ImGui::Selectable("[D] ..", selected_path == 0, ImGuiSelectableFlags_AllowDoubleClick))
		{
			selected_path = 0;
			if (ImGui::IsMouseDoubleClicked(0))
			{
				PopCurrentPath(current_path->GetParent());
			}
		}
	}

	for (size_t i = 0; i < current_path->children.size(); ++i)
	{
		Path* current_path_child = current_path->children[i];
		if (!current_path_child->IsDirectory() && current_path_child->GetExtension() != "scene")
		{
			continue;
		}
		std::string child_path_display_type = current_path_child->IsDirectory() ? "[D]" : "[F]";
		std::string child_path_display_name = child_path_display_type + " " + current_path_child->GetFilename();

		if (ImGui::Selectable(child_path_display_name.c_str(), i == selected_path - 1, ImGuiSelectableFlags_AllowDoubleClick))
		{
			selected_path = i + 1;
			if (current_path_child->IsDirectory())
			{
				if (ImGui::IsMouseDoubleClicked(0))
				{
					PushCurrentPath(current_path_child);
				}
				break;
			}
			else
			{
				if (!ImGui::IsMouseDoubleClicked(0))
				{
					selected_file_name = current_path_child->GetFilename();
				}
				break;
			}
		}
	}

	ImGui::EndChild();
}

void PanelPopupSceneManagement::PushCurrentPath(Path* new_current_path)
{
	current_path = new_current_path;
	path_stack.push_back(current_path);
	selected_path = -1;
}

void PanelPopupSceneManagement::PopCurrentPath(Path* new_current_path)
{
	current_path = new_current_path;
	auto& current_path_pos = std::find(path_stack.begin(), path_stack.end(), current_path);
	path_stack.erase(current_path_pos + 1, path_stack.end());
	selected_path = -1;
}

std::string PanelPopupSceneManagement::GetNormalizedPath()
{
	std::string full_path = current_path->GetFullPath() + "/" + selected_file_name;
	if (Path::GetExtension(selected_file_name) != "scene")
	{
		full_path += ".scene";
	}
	return full_path;
}

void PanelPopupSceneManagement::SetPopupSelection()
{
	has_selected = true;
	ImGui::CloseCurrentPopup();
}

void PanelPopupSceneManagement::ConfirmationPopup()
{
	if (ImGui::BeginPopupModal("Confirmation"))
	{
		ImGui::Text("File already exists. Confirm?");
		if (ImGui::Button("ok"))
		{
			SetPopupSelection();
		}
		ImGui::SameLine();
		if (ImGui::Button("cancel"))
		{
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}

float PanelPopupSceneManagement::GetFrameHeightWithSpacing()
{
	ImGuiContext& g = *GImGui;
	return g.FontSize + g.Style.FramePadding.y * 2.0f + g.Style.ItemSpacing.y;
}

bool PanelPopupSceneManagement::HasSelected() const
{
	return has_selected;
}

std::string PanelPopupSceneManagement::GetSelected() const
{
	return current_path->GetFullPath() + "/" + selected_file_name;
}