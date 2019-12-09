#include "FileExplorerUI.h"

#include "imgui.h"
#include <SDL/SDL.h>
#include <FontAwesome5/IconsFontAwesome5.h>
#include <FontAwesome5/IconsFontAwesome5Brands.h>
#include <Windows.h>
#include <string>

void FileExplorerUI::ShowAssetsFolders() {
	if(ImGui::BeginTabItem(ICON_FA_FOLDER_OPEN " Project") ){

		char buf[256];
		GetCurrentDirectoryA(256, buf);
		std::string current_path(buf);
		current_path += "//";
		WIN32_FIND_DATA FindFileData;
		HANDLE hFind = FindFirstFile("C://Users//Anais//gitHub//LittleOrionEngine//Game//Assets//*", &FindFileData);
		if (hFind == INVALID_HANDLE_VALUE) {
			ImGui::EndTabItem();
			return;
		}

		do {
			if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				ImGui::Text(FindFileData.cFileName);
			}
		} while (FindNextFile(hFind, &FindFileData) != 0);

		FindClose(hFind);

		ImGui::EndTabItem();
	}
}