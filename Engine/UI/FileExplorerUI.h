#ifndef _FILEEXPLORESRUI_H_
#define _FILEEXPLORESRUI_H_

#include <Module/ModuleFileSystem.h>
class FileExplorerUI {

public:
	FileExplorerUI() = default;
	~FileExplorerUI() = default;

	void ShowAssetsFolders();
	void ShowFoldersHierarchy(File & file);

	void ProcessMouseInput(File * file);
	void ShowFilesInExplorer();

	void ShowFileSystemActionsMenu(const File & file);
	void MakeDirectoryFromFile(const File & file) const;
	void CopyFileToSelectedFolder(const char * source) const;

	void FilesDrag() const;

private:
	File * selected_folder;
	File * selected_file;

};
#endif