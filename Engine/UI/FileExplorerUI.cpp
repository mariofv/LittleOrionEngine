#include "FileExplorerUI.h"

#include "imgui.h"
#include <SDL/SDL.h>
#include <FontAwesome5/IconsFontAwesome5.h>
#include <FontAwesome5/IconsFontAwesome5Brands.h>
#include <Windows.h>
#include <string>

void FileExplorerUI::ShowAssetsFolders() {
	if(ImGui::BeginTabItem(ICON_FA_FOLDER_OPEN " Project") ){
		WindowShowFilesInFolder(".//Assets//*");
		ImGui::EndTabItem();
	}
}

void FileExplorerUI::WindowShowFilesInFolder(const char * path) {

	/*if (selected_game_object == game_object)
	{
		flags |= ImGuiTreeNodeFlags_Selected;
	}*/



	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = FindFirstFile(path, &FindFileData);
	if (hFind == INVALID_HANDLE_VALUE) {;
		return;
	}
	do {
		if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY && std::strcmp(FindFileData.cFileName,".")  && std::strcmp(FindFileData.cFileName, ".."))
		{
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_DefaultOpen;
			size_t last_slash_position = std::string(path).find_last_of("//");
			std::string new_path = std::string(path).substr(0, last_slash_position) +"/"+std::string(FindFileData.cFileName)+ "//*";
			int subfolders = GetWindowsSubFolders(new_path.c_str());
			if (subfolders == 0)
			{
				flags |= ImGuiTreeNodeFlags_Leaf;
			}
			if (ImGui::TreeNodeEx(FindFileData.cFileName,flags)) 
			{
				WindowShowFilesInFolder(new_path.c_str());
				ImGui::TreePop();
			}
		}
	} while (FindNextFile(hFind, &FindFileData) != 0);

	FindClose(hFind);

}

int FileExplorerUI::GetWindowsSubFolders(const char * path) {
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = FindFirstFile(path, &FindFileData);

	if (hFind == INVALID_HANDLE_VALUE) {
		return 0;
	}
	int subfolders = 0;
	do {
		if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY && std::strcmp(FindFileData.cFileName, ".") && std::strcmp(FindFileData.cFileName, ".."))
		{
			subfolders++;
		}
	} while (FindNextFile(hFind, &FindFileData) != 0);

	return subfolders;
}