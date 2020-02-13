#ifndef _MODULEEDITOR_H_
#define _MODULEEDITOR_H_

#include "Module.h"
#include "Main/Globals.h"

#include <ImGuizmo.h>

class EngineUI;

class Panel;
class PanelMenuBar;
class PanelToolBar;
class PanelScene;
class PanelGame;
class PanelInspector;
class PanelHierarchy;
class PanelProjectExplorer;
class PanelConsole;
class PanelDebug;
class PanelConfiguration;
class PanelAbout;

class GameObject;

struct ImFont;

class ModuleEditor : public Module
{
public:
	ModuleEditor() = default;
	~ModuleEditor() = default;

	bool Init() override;
	update_status PreUpdate() override;
	update_status Update() override;
	bool CleanUp() override;

	void OpenScene(const std::string &path) const;
	void SaveScene(const std::string &path) const;

	void Render();

	ImFont* GetFont(const Fonts & font) const;

private:
	bool InitImgui();
	void LoadFonts();

public:
	GameObject *selected_game_object = nullptr;

	EngineUI* editor_ui = nullptr;
	PanelMenuBar* menu_bar = nullptr;
	PanelToolBar* toolbar = nullptr;
	PanelScene* scene_panel = nullptr;
	PanelGame* game_panel = nullptr;
	PanelInspector* inspector = nullptr;
	PanelHierarchy* hierarchy = nullptr;
	PanelProjectExplorer* project_explorer = nullptr;
	PanelConsole* console = nullptr;
	PanelDebug* debug_panel = nullptr;
	PanelConfiguration* configuration = nullptr;
	PanelAbout* about = nullptr;

	ImGuizmo::OPERATION gizmo_operation = ImGuizmo::TRANSLATE;

private:
	std::vector<Panel*> panels;
};

#endif //_MODULEEDITOR_H_