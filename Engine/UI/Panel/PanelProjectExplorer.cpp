#include "PanelProjectExplorer.h"

#include "Main/Application.h"

#include <imgui.h>
#include <SDL/SDL.h>
#include <FontAwesome5/IconsFontAwesome5.h>
#include <FontAwesome5/IconsFontAwesome5Brands.h>

#include <algorithm>

PanelProjectExplorer::PanelProjectExplorer()
{
	opened = true;
	enabled = true;
	window_name = ICON_FA_FOLDER_OPEN " Project";
}

void PanelProjectExplorer::Render() 
{
	if(ImGui::Begin(ICON_FA_FOLDER_OPEN " Project", &opened))
	{
		hovered = ImGui::IsWindowHovered();

		if (ImGui::BeginChild("Folder Explorer", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.3f, 0)))
		{
			ShowFileSystemActionsMenu(selected_folder);
			ShowFoldersHierarchy(*App->filesystem->assets_file);
		}
		ImGui::EndChild();

		ImGui::SameLine();
		if (ImGui::BeginChild("File Explorer", ImVec2(0, 0), true)) {
			ShowFileSystemActionsMenu(selected_file);
			ShowFilesInExplorer();
		}
		ImGui::EndChild();
	}
	ImGui::End();
}

void PanelProjectExplorer::ShowFoldersHierarchy(const File & file) 
{
	for (auto & child : file.children)
	{
		if (child->file_type == FileType::DIRECTORY)
		{

			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_DefaultOpen;

			std::string filename = ICON_FA_FOLDER " " + child->filename;
			if (child->sub_folders == 0)
			{
				flags |= ImGuiTreeNodeFlags_Leaf;
			}
			if (selected_folder == child.get())
			{
				flags |= ImGuiTreeNodeFlags_Selected;
				filename = ICON_FA_FOLDER_OPEN " " + child->filename;
			}
			bool expanded = ImGui::TreeNodeEx(filename.c_str(), flags);
			if (expanded) {
				ImGui::PushID(filename.c_str());
				ProcessMouseInput(child.get(), true);
				ShowFoldersHierarchy(*child);
				ImGui::PopID();
				ImGui::TreePop();
			}
		}
	}
}

void PanelProjectExplorer::ShowFilesInExplorer() 
{

	if (selected_folder == nullptr)
	{
		return;
	}
	ImGuiStyle& style = ImGui::GetStyle();
	size_t files_count = 0;

	float window_visible_x2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
	for (auto & file : selected_folder->children)
	{
			std::string item_name;
			std::string filename = std::string(file->filename);
			std::string spaces;
			for (size_t i = 0; i < (filename.size()/2); i++)
			{
				spaces += " ";
			}
			if (file->file_type == FileType::DIRECTORY) {
				item_name = spaces + std::string(ICON_FA_FOLDER "\n " + filename);
			}
			else if(file->file_type == FileType::ARCHIVE)
			{
				item_name = spaces + std::string(ICON_FA_ARCHIVE "\n " + filename);
			}
			else if (file->file_type == FileType::TEXTURE)
			{
				item_name = spaces + std::string(ICON_FA_IMAGE "\n " + filename);
			}
			else if (file->file_type == FileType::MODEL)
			{
				item_name = spaces + std::string(ICON_FA_CUBES "\n " + filename);
			}
			else {
				item_name = spaces + std::string(ICON_FA_BOX "\n " + filename);
			}
			ImVec2 text_sz(ImGui::CalcTextSize(filename.c_str()).x+5,0);
			ImGui::Selectable(item_name.c_str(), selected_file == file.get(),ImGuiSelectableFlags_None,text_sz);
			ProcessMouseInput(file.get(), false);
			FilesDrag();
			++files_count;
			float last_button_x2 = ImGui::GetItemRectMax().x;
			float next_button_x2 = last_button_x2 + style.ItemSpacing.x + text_sz.x; // Expected position if next text was on same line
			if (files_count + 1 < 50 && next_button_x2 < window_visible_x2)
				ImGui::SameLine();
	}
}

void PanelProjectExplorer::ProcessMouseInput(File * file, bool in_folders_windows)
{
	if (ImGui::IsItemHovered())
	{
		if (ImGui::IsMouseClicked(0) && in_folders_windows)
		{
			selected_folder = file;
			selected_file = nullptr;
		}
		else if (ImGui::IsMouseDoubleClicked(0) && file->file_type == FileType::DIRECTORY)
		{
			selected_folder = file;
		}
		else if(ImGui::IsMouseClicked(0) )
		{
			selected_file = file;
		}
	}
}

void PanelProjectExplorer::ShowFileSystemActionsMenu(const File * file)
{
	std::string label("Menu");
	bool changes = false;
	if (ImGui::BeginPopupContextWindow(label.c_str()))
	{
	
		if (ImGui::BeginMenu("Create"))
		{
			if (ImGui::Selectable("Folder"))
			{
				MakeDirectoryFromFile(selected_folder);
				changes = true;
			}
			ImGui::EndMenu();
		}
		if (ImGui::Selectable("Delete"))
		{
			bool success = App->filesystem->Remove(file);
			if (success)
			{
				selected_file = nullptr;
				changes = true;
			}
		}
		if (changes)
		{
			App->filesystem->RefreshFilesHierarchy();
		}
		/*if (ImGui::Selectable("Rename"))
		{
			//TODO
		}
		if (ImGui::Selectable("Copy"))
		{
			//TODO
		}*/

		ImGui::EndPopup();
	}
}

void PanelProjectExplorer::MakeDirectoryFromFile(File * file)
{
	if (file == nullptr)
	{
		return;
	}
	std::shared_ptr<File> new_folder;
	if (!file->file_path.empty() && file->file_type != FileType::DIRECTORY)
	{
		size_t last_slash = file->file_path.find_last_of("/");
		new_folder = std::make_shared<File>(App->filesystem->MakeDirectory(file->file_path.substr(0, last_slash - 1)+"/new Folder"));
	}
	else if(!file->file_path.empty())
	{
		new_folder = std::make_shared<File>(App->filesystem->MakeDirectory(file->file_path+"/new Folder"));
	}
	else if(!selected_folder->file_path.empty()){
		new_folder = std::make_shared<File>(App->filesystem->MakeDirectory(selected_folder->file_path+ "/new Folder"));
	}
	file->children.push_back(new_folder);
	selected_folder = new_folder.get();
}

void PanelProjectExplorer::CopyFileToSelectedFolder(const char * source) const
{
	std::string source_string(source);
	std::replace(source_string.begin(), source_string.end(), '\\', '/');
	std::string file_name = source_string.substr(source_string.find_last_of('/'), -1);
	std::string destination;
	if (selected_folder == nullptr)
	{
		destination = "Assets"+ file_name;
	}
	else
	{
		destination = selected_folder->file_path  + file_name;
	}
	App->filesystem->Copy(source, destination.c_str());
}

void PanelProjectExplorer::FilesDrag() const
{
	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
	{
		ImGui::SetDragDropPayload("DND_File", &selected_file, sizeof(File*));
		ImGui::Text("Dragging %s", selected_file->filename.c_str());
		ImGui::EndDragDropSource();
	}
}