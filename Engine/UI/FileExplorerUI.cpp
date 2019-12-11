#include "FileExplorerUI.h"
#include <Application.h>
#include <Module/ModuleFileSystem.h>

#include "imgui.h"
#include <SDL/SDL.h>
#include <FontAwesome5/IconsFontAwesome5.h>
#include <FontAwesome5/IconsFontAwesome5Brands.h>


void FileExplorerUI::ShowAssetsFolders() {
	if(ImGui::BeginTabItem(ICON_FA_FOLDER_OPEN " Project") ){
		if (ImGui::BeginChild("Folder Explorer", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.3f, 260)))
		{
			WindowShowFilesInFolder(".//Assets//*");
		}
		ImGui::EndChild();

		ImGui::SameLine();
		if (ImGui::BeginChild("File Explorer", ImVec2(0, 260), true)) {
			ShowFilesInExplorer(selected_folder);
		}
		ImGui::EndChild();
		ImGui::EndTabItem();
	}
}

void FileExplorerUI::WindowShowFilesInFolder(const char * path) {

	std::vector<std::shared_ptr<ModuleFileSystem::File>> files;
	std::string string_path(path);
	App->filesystem->GetAllFilesInPath(string_path, files);
	for (auto & file : files )
	{
		if (file->file_type == ModuleFileSystem::FileType::DIRECTORY) {
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_DefaultOpen;

			size_t last_slash_position = string_path.find_last_of("//");
			std::string new_path = string_path.substr(0, last_slash_position) + "/" + file->filename + "//*";

			size_t subfolders = App->filesystem->GetNumberOfSubFolders(new_path);
			std::string filename = ICON_FA_FOLDER " " + file->filename;
			if (subfolders == 0)
			{
				flags |= ImGuiTreeNodeFlags_Leaf;
			}
			if (selected_folder == new_path)
			{
				flags |= ImGuiTreeNodeFlags_Selected;
				filename = ICON_FA_FOLDER_OPEN " " + file->filename;
			}
			bool expanded = ImGui::TreeNodeEx(filename.c_str(), flags);
			if (expanded) {
				ImGui::PushID(filename.c_str());
				ProcessMouseInput(new_path);
				WindowShowFilesInFolder(new_path.c_str());
				ImGui::PopID();
				ImGui::TreePop();
			}
		}
	}
}




void FileExplorerUI::ShowFilesInExplorer(std::string & folder_path) {
	std::vector<std::shared_ptr<ModuleFileSystem::File>> files;
	std::string string_path(folder_path);
	App->filesystem->GetAllFilesInPath(string_path, files);
	
	ImGuiStyle& style = ImGui::GetStyle();
	size_t files_count = 0;
	ImVec2 text_sz(40, 40);
	float window_visible_x2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;

	for (auto & file : files)
	{
			bool outsideWindow = false;
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
			ImGui::Text(item_name.c_str(), text_sz);
			++files_count;
			float last_button_x2 = ImGui::GetItemRectMax().x;
			float next_button_x2 = last_button_x2 + style.ItemSpacing.x + text_sz.x; // Expected position if next text was on same line
			if (files_count + 1 < 50 && next_button_x2 < window_visible_x2)
				ImGui::SameLine();
	}
}
void FileExplorerUI::ProcessMouseInput(std::string & path)
{
	if (ImGui::IsItemHovered())
	{
		if (ImGui::IsMouseClicked(0))
		{
			selected_folder = path;
		}
	}
}