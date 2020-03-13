#ifndef _PANELPROJECTEXPLORER_H_
#define _PANELPROJECTEXPLORER_H_

#include "UI/Panel/Panel.h"
#include "Module/ModuleFileSystem.h"

class PanelProjectExplorer : public Panel
{

public:
	PanelProjectExplorer();
	~PanelProjectExplorer() = default;

	void Render() override;

	void ShowFoldersHierarchy(const File& file);

	void ProcessMouseInput(File* file);
	void ProcessResourceMouseInput(File* file);
	void ShowFilesInExplorer();

	void ShowFileSystemActionsMenu(const File* file);
	void MakeDirectoryFromFile(File* file);
	void CopyFileToSelectedFolder(const char* source) const;

	void FilesDrop() const;
	void ResourceDragSource(File* file) const;

private:
	void InitResourceExplorerDockspace();

	void ShowFileIcon(File* file);

private:
	float file_size = 100.f;

	File* selected_folder = nullptr;
	File* selected_file = nullptr;

	ImGuiID project_explorer_dockspace_id;
};
#endif //_PANELPROJECTEXPLORER_H_