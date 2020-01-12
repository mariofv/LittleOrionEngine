#ifndef _FILEEXPLORESRUI_H_
#define _FILEEXPLORESRUI_H_

#include <Module/ModuleFileSystem.h>
class FileExplorerUI {

public:
	FileExplorerUI() = default;
	~FileExplorerUI() = default;

	void ShowAssetsFolders();
	void ShowFoldersHierarchy(const File & file);

	void ProcessMouseInput(File * file, bool in_folders_windows);
	void ShowFilesInExplorer();

	void ShowFileSystemActionsMenu(const File * file);
	void MakeDirectoryFromFile(File * file);
	void CopyFileToSelectedFolder(const char * source) const;

	void FilesDrag() const;

private:
	File * selected_folder;
	File * selected_file;

};
#endif