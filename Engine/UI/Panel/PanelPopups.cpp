#include "PanelPopups.h"

#include "Component/ComponentMeshRenderer.h"
#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleEditor.h"
#include "Module/ModuleResourceManager.h"
#include "Module/ModuleTexture.h"

#include <FontAwesome5/IconsFontAwesome5.h>
#include <imgui.h>

PanelPopups::PanelPopups()
{
	opened = true;
	enabled = true;
	window_name = "Popups";
}

void PanelPopups::Render()
{
	RenderAssetsLoadingPopup();
	RenderModelSelectorPopup();
}

void PanelPopups::RenderAssetsLoadingPopup()
{
	if (!show_assets_loading_popup)
	{
		show_assets_loading_popup = true;
		ImGui::OpenPopup("Loading Assets");
	}

	if (ImGui::BeginPopupModal("Loading Assets", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar))
	{
		ImGui::Text("Loading Assets");
		float progress = (float)App->resources->thread_comunication.loaded_items / App->resources->thread_comunication.total_items;
		ImGui::ProgressBar(progress);

		if (App->resources->thread_comunication.finished_loading)
		{
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}

void PanelPopups::RenderModelSelectorPopup()
{
	if (show_mesh_selector_popup)
	{
		ImGui::OpenPopup("Select Mesh");
		show_mesh_selector_popup = false;
		mesh_selector_opened = true;
	
		// TODO: Change this when ModuleResourceManagement is refactored.
		mesh_files = std::vector<std::shared_ptr<File>>();
		File meshes_folder(MESHES_PATH);
		for (auto& mesh_folder : meshes_folder.children)
		{
			File* mesh_folder_ptr = mesh_folder.get();
			for (auto& mesh_file : mesh_folder_ptr->children)
			{
				if (mesh_file.get()->file_type == FileType::MODEL)
				{
					mesh_files.push_back(mesh_file);
				}
			}
		}
	}

	ImGui::SetNextWindowSize(ImVec2(mesh_icon_size * 4.5, mesh_icon_size * 2 * 1.1f));

	if (ImGui::BeginPopupModal("Select Mesh", &mesh_selector_opened))
	{

		ImVec2 available_region = ImGui::GetContentRegionAvail();
		int files_per_line = available_region.x / mesh_icon_size;

		int current_line = 0;
		int current_file_in_line = 0;

		for (auto & file : mesh_files)
		{
			ImGui::PushID(current_line * files_per_line + current_file_in_line);
			ShowMeshIcon(file.get());
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

		ImGui::EndPopup();
	}
}


void PanelPopups::ShowMeshIcon(File* file)
{
	std::string filename = std::string(file->filename_no_extension);
	if (ImGui::BeginChild(filename.c_str(), ImVec2(mesh_icon_size, mesh_icon_size), selected_mesh == file, ImGuiWindowFlags_NoDecoration))
	{
		ProcessMeshMouseInput(file);

		ImGui::SetCursorPosX((ImGui::GetWindowWidth() - 0.75 * mesh_icon_size) * 0.5f);
		ImGui::Image((void *)App->texture->whitefall_texture_id, ImVec2(0.75*mesh_icon_size, 0.75 * mesh_icon_size)); // TODO: Substitute this with resouce thumbnail
		ImGui::Spacing();

		float text_width = ImGui::CalcTextSize(filename.c_str()).x;
		if (text_width < mesh_icon_size)
		{
			ImGui::SetCursorPosX((ImGui::GetWindowWidth() - text_width) * 0.5f);
			ImGui::Text(filename.c_str());
		}
		else
		{
			int character_width = text_width / filename.length();
			int string_position_wrap = mesh_icon_size / character_width - 5;
			assert(string_position_wrap < filename.length());
			std::string wrapped_filename = filename.substr(0, string_position_wrap) + "...";
			ImGui::Text(wrapped_filename.c_str());
		}
	}
	ImGui::EndChild();
}

void PanelPopups::ProcessMeshMouseInput(File * file)
{
	if (ImGui::IsWindowHovered())
	{
		if (ImGui::IsMouseClicked(0))
		{
			if (selected_mesh != file)
			{
				selected_mesh = file;
				ChangeSelectedObjectMesh();
			}
		}

		if (ImGui::IsMouseDoubleClicked(0))
		{
			ImGui::CloseCurrentPopup();
		}
	}
}

void PanelPopups::ChangeSelectedObjectMesh() const
{
	GameObject* selected_gameobject = App->editor->selected_game_object;
	Component* selected_gameobject_mesh_renderer_component = selected_gameobject->GetComponent(Component::ComponentType::MESH_RENDERER);
	assert(selected_gameobject_mesh_renderer_component != nullptr);
	ComponentMeshRenderer* selected_gameobject_mesh_renderer = static_cast<ComponentMeshRenderer*>(selected_gameobject_mesh_renderer_component);

	std::shared_ptr<Mesh> new_mesh = App->resources->Load<Mesh>(selected_mesh->file_path.c_str());
	selected_gameobject_mesh_renderer->SetMesh(new_mesh);
}