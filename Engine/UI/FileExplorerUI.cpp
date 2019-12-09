#include "FileExplorerUI.h"

#include "imgui.h"
#include <SDL/SDL.h>
#include <FontAwesome5/IconsFontAwesome5.h>
#include <FontAwesome5/IconsFontAwesome5Brands.h>
#include <Windows.h>


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

	WIN32_FIND_DATA find_file_data;
	HANDLE handle_find = FindFirstFile(path, &find_file_data);
	if (handle_find == INVALID_HANDLE_VALUE) {;
		return;
	}
	do {
		if (find_file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY && std::strcmp(find_file_data.cFileName,".")  && std::strcmp(find_file_data.cFileName, ".."))
		{
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_DefaultOpen;
			size_t last_slash_position = std::string(path).find_last_of("//");
			std::string new_path = std::string(path).substr(0, last_slash_position) +"/"+std::string(find_file_data.cFileName)+ "//*";
			int subfolders = GetWindowsSubFolders(new_path.c_str());
			std::string filename = ICON_FA_FOLDER " "+std::string(find_file_data.cFileName);
			if (subfolders == 0)
			{
				flags |= ImGuiTreeNodeFlags_Leaf;
			}
			if (selected_folder == new_path)
			{
				flags |= ImGuiTreeNodeFlags_Selected;
				filename = ICON_FA_FOLDER_OPEN " " + std::string(find_file_data.cFileName);
			}
			if (ImGui::TreeNodeEx(filename.c_str(),flags))
			{
				ProcessMouseInput(new_path);
				WindowShowFilesInFolder(new_path.c_str());
			}
			ImGui::TreePop();
		}
	} while (FindNextFile(handle_find, &find_file_data) != 0);

	FindClose(handle_find);

}

int FileExplorerUI::GetWindowsSubFolders(const char * path) {
	WIN32_FIND_DATA find_file_data;
	HANDLE handle_find = FindFirstFile(path, &find_file_data);

	if (handle_find == INVALID_HANDLE_VALUE) {
		return 0;
	}
	int subfolders = 0;
	do {
		if (find_file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY && std::strcmp(find_file_data.cFileName, ".") && std::strcmp(find_file_data.cFileName, ".."))
		{
			subfolders++;
		}
	} while (FindNextFile(handle_find, &find_file_data) != 0);

	return subfolders;
}


void FileExplorerUI::ShowFilesInExplorer(std::string & folder_path) {
	WIN32_FIND_DATA find_file_data;
	HANDLE handle_find = FindFirstFile(folder_path.c_str(), &find_file_data);
	if (handle_find == INVALID_HANDLE_VALUE) {
		return;
	}
	do {
		if (std::strcmp(find_file_data.cFileName, ".") && std::strcmp(find_file_data.cFileName, ".."))
		{
			bool outsideWindow = false;
			std::string item_name;
			size_t text_size = ImGui::CalcTextSize(find_file_data.cFileName).x;
			if (find_file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				item_name = std::string(ICON_FA_FOLDER "\n " + std::string(find_file_data.cFileName));
				
			}
			else
			{
				item_name = std::string(ICON_FA_BOX "\n " + std::string(find_file_data.cFileName));
			}
			ImGui::Text(item_name.c_str());
			size_t last_item_width = ImGui::CalcItemWidth();

			ImGui::SameLine();
		}
	} while (FindNextFile(handle_find, &find_file_data) != 0);

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