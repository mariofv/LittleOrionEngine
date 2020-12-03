#ifndef _PANELPROJECTEXPLORER_H_
#define _PANELPROJECTEXPLORER_H_

#include "EditorUI/Panel/Panel.h"
#include "Module/ModuleFileSystem.h"
#include <unordered_map>
class Metafile;
class ModelMetafile;
class Texture;
class GameObject;
class PanelProjectExplorer : public Panel
{

public:
	PanelProjectExplorer();
	~PanelProjectExplorer() = default;

	void Render() override;

	void ShowFoldersHierarchy(const Path& file);
	void ShowFoldersHierarchySearch(const Path& file);

	void ProcessMouseInput(Path* file_path);
	void ProcessResourceMouseInput(Path* metafile_path, Metafile* metafile);
	void ShowFilesInExplorer();
	void SearchFilesInExplorer(Path* file);
	

	void CalculateNextLinePosition(int& current_file_in_line, int files_per_line, int& current_line);

	void ShowFileSystemActionsMenu(Path* path);

	void FilesDrop() const;

	void CreatePrefabInSelectedFolder(GameObject * incoming_game_object) const;

	void ResourceDragSource(const Metafile* file) const;
	void ResourceDropTarget(Path* folder_path) const;
	void ResourceDropFromOutside(const std::string& dropped_filedir);

	bool IsOneOfMyChildrens(Path* path) const;

private:
	void InitResourceExplorerDockspace();

	void ShowMetafile(Path* file, Metafile* metafile, const std::string& filename);
	void ShowMetafileIcon(Metafile* metafile);

	size_t GetResourcePreviewImage(uint32_t uuid);
	void ShowSearchedFiles();

private:
	float file_size_width = 100.f;
	float file_size_height = 150.f;

	Path* selected_folder = nullptr;
	Path* selected_file = nullptr;

	ImGuiID project_explorer_dockspace_id;

	std::unordered_map<uint32_t,std::shared_ptr<Texture>> project_explorer_icon_cache;

	std::vector<Path*> searching_file_paths;
public:
	ModelMetafile* opened_model = nullptr;
	std::string searching_file;

};
#endif //_PANELPROJECTEXPLORER_H_