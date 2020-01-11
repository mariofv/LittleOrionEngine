#ifndef _FILEEXPLORESRUI_H_
#define _FILEEXPLORESRUI_H_

#include <Module/ModuleFileSystem.h>
class FileExplorerUI {

public:
	FileExplorerUI() = default;
	~FileExplorerUI() = default;

	void ShowAssetsFolders();
	void WindowShowFilesInFolder(File & file);

	void ProcessMouseInput(File * file);
	void ShowFilesInExplorer(std::string & folder_path);

	void ShowFileSystemActionsMenu(const File & file);
	void MakeDirectoryFromFile(const File & file) const;
	void CopyFileToSelectedFolder(const char * source) const;

	void FilesDrag() const;

private:
	File * selected_folder;
	File * selected_file;
	std::vector<std::shared_ptr<File>> files_in_selected_folder;

};
#endif