#include "FileExplorerUI.h"
#include <Application.h>

#include "imgui.h"
#include <SDL/SDL.h>
#include <FontAwesome5/IconsFontAwesome5.h>
#include <FontAwesome5/IconsFontAwesome5Brands.h>

void FileExplorerUI::ShowAssetsFolders() {
	if(ImGui::BeginTabItem(ICON_FA_FOLDER_OPEN " Assets") ){

		if (ImGui::BeginChild("Folder Explorer", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.3f, 260)))
		{
			ShowFileSystemActionsMenu(selected_folder);
			WindowShowFilesInFolder(*App->filesystem->root_file);
		}
		ImGui::EndChild();

		ImGui::SameLine();
		if (ImGui::BeginChild("File Explorer", ImVec2(0, 260), true)) {
			ShowFileSystemActionsMenu(selected_file);
			ShowFilesInExplorer(selected_folder.file_path);
		}
		ImGui::EndChild();
		ImGui::EndTabItem();
	}
}

void FileExplorerUI::WindowShowFilesInFolder(ModuleFileSystem::File & file) {

	for (auto & child : file.childs )
	{
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_DefaultOpen;

		size_t subfolders = App->filesystem->GetNumberOfSubFolders(child->file_path);
		std::string filename = ICON_FA_FOLDER " " + child->filename;
		if (subfolders == 0)
		{
			flags |= ImGuiTreeNodeFlags_Leaf;
		}
		if (selected_folder == *child)
		{
			flags |= ImGuiTreeNodeFlags_Selected;
			filename = ICON_FA_FOLDER_OPEN " " + child->filename;
		}
		bool expanded = ImGui::TreeNodeEx(filename.c_str(), flags);
		if (expanded) {
			ImGui::PushID(filename.c_str());
			ProcessMouseInput(*child);
			WindowShowFilesInFolder(*child);
			ImGui::PopID();
			ImGui::TreePop();
		}
	}
}

void FileExplorerUI::ShowFilesInExplorer(std::string & folder_path) {

	ImGuiStyle& style = ImGui::GetStyle();
	size_t files_count = 0;

	float window_visible_x2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;

	for (auto & file : files_in_selected_folder)
	{
			std::string item_name;
			std::string filename = std::string(file->filename);
			std::string spaces;
			for (size_t i = 0; i < (filename.size()/2); i++)
			{
				spaces += " ";
			}
			if (file->file_type == ModuleFileSystem::FileType::DIRECTORY) {
				item_name = spaces + std::string(ICON_FA_FOLDER "\n " + filename);
			}
			else if(file->file_type == ModuleFileSystem::FileType::ARCHIVE)
			{
				item_name = spaces + std::string(ICON_FA_ARCHIVE "\n " + filename);
			}
			else if (file->file_type == ModuleFileSystem::FileType::TEXTURE)
			{
				item_name = spaces + std::string(ICON_FA_IMAGE "\n " + filename);
			}
			else if (file->file_type == ModuleFileSystem::FileType::MODEL)
			{
				item_name = spaces + std::string(ICON_FA_CUBES "\n " + filename);
			}
			else {
				item_name = spaces + std::string(ICON_FA_BOX "\n " + filename);
			}
			ImVec2 text_sz(ImGui::CalcTextSize(filename.c_str()).x+5,0);
			ImGui::Selectable(item_name.c_str(), selected_file == *file,ImGuiSelectableFlags_None,text_sz);
			ProcessMouseInput(*file);
			++files_count;
			float last_button_x2 = ImGui::GetItemRectMax().x;
			float next_button_x2 = last_button_x2 + style.ItemSpacing.x + text_sz.x; // Expected position if next text was on same line
			if (files_count + 1 < 50 && next_button_x2 < window_visible_x2)
				ImGui::SameLine();
	}
}
void FileExplorerUI::ProcessMouseInput(ModuleFileSystem::File file)
{
	if (ImGui::IsItemHovered())
	{
		if (ImGui::IsMouseClicked(0) && file.file_type == ModuleFileSystem::FileType::DIRECTORY)
		{
			selected_folder = file;
			selected_file = ModuleFileSystem::File();
			files_in_selected_folder.clear();
			App->filesystem->GetAllFilesInPath(selected_folder.file_path, files_in_selected_folder);
		}
		else if(ImGui::IsMouseClicked(0))
		{
			selected_file = file;
		}
	}
}

void FileExplorerUI::ShowFileSystemActionsMenu(const ModuleFileSystem::File & file)
{
	std::string label("Menu");

	if (ImGui::BeginPopupContextWindow(label.c_str()))
	{
	
		if (ImGui::BeginMenu("Create"))
		{
			if (ImGui::Selectable("Folder"))
			{
				MakeDirectoryFromFile(file);
			}
			if (ImGui::Selectable("Empty File"))
			{
				char * test = "This is a test text";
				std::string new_empty_file = file.file_path + "//example.txt";
				App->filesystem->Save(new_empty_file.c_str(), test, sizeof("This is a test text"),false);
				size_t size;
				char * test2 = App->filesystem->Load(new_empty_file.c_str(), size);
				APP_LOG_INFO("Read: %s",test2);
			}
			ImGui::EndMenu();
		}
		if (ImGui::Selectable("Delete"))
		{
			bool removed = App->filesystem->Remove(file);
		}
		if (ImGui::Selectable("Rename"))
		{
		}
		if (ImGui::Selectable("Copy"))
		{
		}

		files_in_selected_folder.clear();
		App->filesystem->GetAllFilesInPath(selected_folder.file_path, files_in_selected_folder);
		App->filesystem->RefreshFilesHierarchy();
		ImGui::EndPopup();
	}
}

void FileExplorerUI::MakeDirectoryFromFile(const ModuleFileSystem::File & file) const
{
	if (!file.file_path.empty() && file.file_type != ModuleFileSystem::FileType::DIRECTORY)
	{
		size_t last_slash = file.file_path.find_last_of("//");
		App->filesystem->MakeDirectory(file.file_path.substr(0, last_slash - 1), "new Folder");
	}
	else if(!file.file_path.empty())
	{
		App->filesystem->MakeDirectory(file.file_path, "new Folder");
	}
	else if(!selected_folder.file_path.empty()){
		App->filesystem->MakeDirectory(selected_folder.file_path, "new Folder");
	}
}