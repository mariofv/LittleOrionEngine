#ifndef _PANELRESOURCESEXPLORER_H_
#define _PANELRESOURCESEXPLORER_H_

#include "UI/Panel/Panel.h"
#include "Module/ModuleFileSystem.h"

class PanelResourcesExplorer : public Panel
{

public:
	PanelResourcesExplorer();
	~PanelResourcesExplorer() = default;

	void Render() override;

	void ShowFoldersHierarchy(const File& file);

	void ProcessMouseInput(File* file, bool in_folders_windows);
	void ShowFilesInExplorer();

	void ShowFileSystemActionsMenu(const File* file);
	void MakeDirectoryFromFile(File* file);
	void CopyFileToSelectedFolder(const char* source) const;

	void FilesDrag() const;

private:
	File* selected_folder = nullptr;
	File* selected_file = nullptr;

};
#endif //_PANELRESOURCESEXPLORER_H_