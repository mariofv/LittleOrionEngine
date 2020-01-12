#include "FileExplorerUI.h"
#include <Application.h>

#include "imgui.h"
#include <SDL/SDL.h>
#include <FontAwesome5/IconsFontAwesome5.h>
#include <FontAwesome5/IconsFontAwesome5Brands.h>

#include <algorithm>

void FileExplorerUI::ShowAssetsFolders() {
	if(ImGui::BeginTabItem(ICON_FA_FOLDER_OPEN " Assets") ){

		if (ImGui::BeginChild("Folder Explorer", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.3f, 260)))
		{
			ShowFileSystemActionsMenu(*selected_folder);
			ShowFoldersHierarchy(*App->filesystem->assets_file);
		}
		ImGui::EndChild();

		ImGui::SameLine();
		if (ImGui::BeginChild("File Explorer", ImVec2(0, 260), true)) {
			ShowFileSystemActionsMenu(*selected_file);
			ShowFilesInExplorer();
		}
		ImGui::EndChild();
		ImGui::EndTabItem();
	}
}

void FileExplorerUI::ShowFoldersHierarchy(File & file) {

	for (auto & child : file.children)
	{
		if (child->file_type == FileType::DIRECTORY)
		{

			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_DefaultOpen;

			std::string filename = ICON_FA_FOLDER " " + child->filename;
			if (child->subFolders == 0)
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
				ProcessMouseInput(child.get());
				ShowFoldersHierarchy(*child);
				ImGui::PopID();
				ImGui::TreePop();
			}
		}
	}
}

void FileExplorerUI::ShowFilesInExplorer() {

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
			ProcessMouseInput(file.get());
			FilesDrag();
			++files_count;
			float last_button_x2 = ImGui::GetItemRectMax().x;
			float next_button_x2 = last_button_x2 + style.ItemSpacing.x + text_sz.x; // Expected position if next text was on same line
			if (files_count + 1 < 50 && next_button_x2 < window_visible_x2)
				ImGui::SameLine();
	}
}
void FileExplorerUI::ProcessMouseInput(File * file)
{
	if (ImGui::IsItemHovered())
	{
		if (ImGui::IsMouseClicked(0) && file->file_type == FileType::DIRECTORY)
		{
			selected_folder = file;
			selected_file = nullptr;
		}
		else if(ImGui::IsMouseClicked(0))
		{
			selected_file = file;
		}
	}
}

void FileExplorerUI::ShowFileSystemActionsMenu(const File & file)
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
				char * load_test = App->filesystem->Load(new_empty_file.c_str(), size);
				APP_LOG_INFO("Read: %s", load_test);
				free(load_test);
			}
			ImGui::EndMenu();
		}
		if (ImGui::Selectable("Delete"))
		{
			bool removed = App->filesystem->Remove(file);
		}
		if (ImGui::Selectable("Rename"))
		{
			//TODO
		}
		if (ImGui::Selectable("Copy"))
		{
			//TODO
		}

		ImGui::EndPopup();
	}
}

void FileExplorerUI::MakeDirectoryFromFile(const File & file) const
{
	if (!file.file_path.empty() && file.file_type != FileType::DIRECTORY)
	{
		size_t last_slash = file.file_path.find_last_of("/");
		App->filesystem->MakeDirectory(file.file_path.substr(0, last_slash - 1)+"/new Folder");
	}
	else if(!file.file_path.empty())
	{
		App->filesystem->MakeDirectory(file.file_path+"/new Folder");
	}
	else if(!selected_folder->file_path.empty()){
		App->filesystem->MakeDirectory(selected_folder->file_path+ "/new Folder");
	}
}

void FileExplorerUI::CopyFileToSelectedFolder(const char * source) const
{
	std::string source_string(source);
	std::replace(source_string.begin(), source_string.end(), '\\', '/');
	std::string file_name = source_string.substr(source_string.find_last_of('/'), -1);
	std::string destination;
	if (selected_folder == nullptr)
	{
		destination = "Assets"+ file_name;
	}
	else;
	{
		destination = selected_folder->file_path  + file_name;
	}
	App->filesystem->Copy(source, destination.c_str());
}

void FileExplorerUI::FilesDrag() const
{
	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
	{
		ImGui::SetDragDropPayload("DND_File", &selected_file, sizeof(File*));
		ImGui::Text("Dragging %s", selected_file->filename.c_str());
		ImGui::EndDragDropSource();
	}
}