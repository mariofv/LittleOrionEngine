#ifndef _FILEEXPLORESRUI_H_
#define _FILEEXPLORESRUI_H_

#include <string>
class FileExplorerUI {

public:
	FileExplorerUI() = default;
	~FileExplorerUI() = default;

	void ShowAssetsFolders();
	void WindowShowFilesInFolder(const char * path);
	int GetWindowsSubFolders(const char * path);

	void ProcessMouseInput(std::string & path);
	void ShowFilesInExplorer(std::string & folder_path);

private:
	std::string selected_folder;
};
#endif