#ifndef _PANELPOPUPSCENEMANAGEMENT_H_
#define _PANELPOPUPSCENEMANAGEMENT_H_

#include "EditorUI/Panel/Panel.h"

#include <string>

#include <imgui.h>
#include <imfilebrowser.h>

class PanelPopupSceneManagement : public Panel
{
public:
	PanelPopupSceneManagement();
	~PanelPopupSceneManagement() = default;

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

#endif //_PANELPOPUPSCENEMANAGEMENT_H_