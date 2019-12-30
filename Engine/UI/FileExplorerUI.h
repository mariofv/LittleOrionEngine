#ifndef _FILEEXPLORESRUI_H_
#define _FILEEXPLORESRUI_H_

#include <Module/ModuleFileSystem.h>
class FileExplorerUI {

public:
	FileExplorerUI() = default;
	~FileExplorerUI() = default;

	void ShowAssetsFolders();
	void WindowShowFilesInFolder(ModuleFileSystem::File & file);

	void ProcessMouseInput(ModuleFileSystem::File * file);
	void ShowFilesInExplorer(std::string & folder_path);

	void ShowFileSystemActionsMenu(const ModuleFileSystem::File & file);
	void MakeDirectoryFromFile(const ModuleFileSystem::File & file) const;
	void CopyFileToSelectedFolder(const char * source) const;

	void FilesDrag() const;

private:
	ModuleFileSystem::File * selected_folder;
	ModuleFileSystem::File * selected_file;
	std::vector<std::shared_ptr<ModuleFileSystem::File>> files_in_selected_folder;

};
#endif