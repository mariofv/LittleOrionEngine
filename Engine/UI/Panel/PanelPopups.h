#ifndef _PANELPOPUPS_H_
#define _PANELPOPUPS_H_

#include "Panel.h"
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
	bool create_script_shown = false;
	bool save_scene_shown = false;
	bool load_scene_shown = false;

private:
	bool assets_loading_popup_shown = false;
	ImGui::FileBrowser fileDialog;
};

#endif //_PANELPOPUPS_H_