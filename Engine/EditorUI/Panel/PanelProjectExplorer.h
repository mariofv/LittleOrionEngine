#ifndef _PANELPROJECTEXPLORER_H_
#define _PANELPROJECTEXPLORER_H_

#include "EditorUI/Panel/Panel.h"
#include "Module/ModuleFileSystem.h"

class PanelProjectExplorer : public Panel
{

public:
	PanelProjectExplorer();
	~PanelProjectExplorer() = default;

	void Render() override;

	void ShowFoldersHierarchy(const Path& file);

	void ProcessMouseInput(Path* file);
	void ProcessResourceMouseInput(Path* file);
	void ShowFilesInExplorer();

	void ShowFileSystemActionsMenu(const Path* file);
	void MakeDirectoryFromFile(Path* file);
	void CopyFileToSelectedFolder(const char* source) const;

	void FilesDrop() const;
	void ResourceDragSource(Path* file) const;

private:
	void InitResourceExplorerDockspace();

	void ShowFileIcon(Path* file);

private:
	float file_size_width = 100.f;
	float file_size_height = 150.f;

	Path* selected_folder = nullptr;
	Path* selected_file = nullptr;

	ImGuiID project_explorer_dockspace_id;
};
#endif //_PANELPROJECTEXPLORER_H_