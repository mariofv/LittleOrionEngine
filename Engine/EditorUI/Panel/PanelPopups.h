#ifndef _PANELPOPUPS_H_
#define _PANELPOPUPS_H_

#include "Panel.h"
#include "PopupsPanel/PanelPopupMaterialSelector.h"
#include "PopupsPanel/PanelPopupMeshSelector.h"

#include <imgui.h>
#include <imfilebrowser.h>
#include <string>

class PanelPopups : public Panel
{
public:
	PanelPopups();
	~PanelPopups() = default;

	void Render() override;

	void CreateScript();

	void SaveScene();
	void LoadScene();

private:
	void RenderAssetsLoadingPopup();

public:
	PanelPopupMaterialSelector material_selector_popup;
	PanelPopupMeshSelector mesh_selector_popup;

	bool create_script_shown = false;
	bool save_scene_shown = false;
	bool load_scene_shown = false;
	bool show_assets_loading_popup = false;

	ImGui::FileBrowser fileDialog; private:

};

#endif //_PANELPOPUPS_H_