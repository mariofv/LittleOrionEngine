#ifndef _PANELPOPUPSCENEMANAGEMENTAUX_H_
#define _PANELPOPUPSCENEMANAGEMENTAUX_H_

#include "EditorUI/Panel/Panel.h"

#include <string>

#include <imgui.h>
#include <imfilebrowser.h>

class PanelPopupSceneManagementAux : public Panel
{
public:
	PanelPopupSceneManagementAux();
	~PanelPopupSceneManagementAux() = default;

	void Render() override;;
	
	void RenderSaveScene();
	void RenderLoadScene();

public:
	bool save_scene_shown = false;
	bool load_scene_shown = false;

private:
	ImGui::FileBrowser save_scene_popup; 
	ImGui::FileBrowser load_scene_popup; 

};

#endif //_PANELPOPUPSCENEMANAGEMENTAUX_H_