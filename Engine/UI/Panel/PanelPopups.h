#ifndef _PANELPOPUPS_H_
#define _PANELPOPUPS_H_

#include "Panel.h"
#include "Module/ModuleFileSystem.h"

#include <string>

class PanelPopups : public Panel
{
public:
	PanelPopups();
	~PanelPopups() = default;

	void Render() override;

private:
	void RenderAssetsLoadingPopup();
	void RenderModelSelectorPopup();

	void ShowMeshIcon(File* file);
	void ProcessMeshMouseInput(File * file);
	void ChangeSelectedObjectMesh() const;

public:
	bool show_assets_loading_popup = false;

	bool show_mesh_selector_popup = false;
	bool mesh_selector_opened = true;
	float mesh_icon_size = 125.f;
	File* selected_mesh = nullptr;
	std::vector<std::shared_ptr<File>> mesh_files;
};

#endif //_PANELPOPUPS_H_