#ifndef _FILEEXPLORESRUI_H_
#define _FILEEXPLORESRUI_H_

#include <Module/ModuleFileSystem.h>
class FileExplorerUI {

public:
	FileExplorerUI() = default;
	~FileExplorerUI() = default;

	void ShowAssetsFolders();
	void WindowShowFilesInFolder(const char * path);

	void ProcessMouseInput(ModuleFileSystem::File file);
	void ShowFilesInExplorer(std::string & folder_path);

	void ShowFileSystemActionsMenu(const ModuleFileSystem::File & file);

private:
	ModuleFileSystem::File selected_file;
};
#endif