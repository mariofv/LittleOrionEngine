#include "FileExplorerUI.h"

#include "imgui.h"
#include <SDL/SDL.h>
#include <FontAwesome5/IconsFontAwesome5.h>
#include <FontAwesome5/IconsFontAwesome5Brands.h>

void FileExplorerUI::ShowAssetsFolders() {
	if(ImGui::BeginTabItem(ICON_FA_FOLDER_OPEN " Project") ){

		ImGui::EndTabItem();
	}
}