#ifndef _ENGINEUI_H_
#define _ENGINEUI_H_

#include "Application.h"
#include "Globals.h"
#include "FileExplorerUI.h"

class EngineUI
{
public:
	EngineUI() = default;
	~EngineUI() = default;

	void ShowEngineUI();
	void InitUI();

private:

	void ShowMainMenu();
	void ShowFileMenu();
	void ShowViewMenu();
	void ShowHelpMenu();

	void ShowSceneControls();

	void ShowHierarchyWindow();
	
	void ShowMainViewWindow();
	void ShowSceneTab();
	void ShowGameTab();
	void ShowEmptyGameTab() const;

	void ShowInspectorWindow();
	void ShowAddNewComponentButton();

	void ShowConfigurationWindow();
	void ShowHardware() const;

	void ShowAboutWindow();
	
	void ShowFileAndConsoleWindows();

	void ShowPopups() const;

	void MenuURL(const std::string& text, const std::string& link);

public:
	#define MAIN_MENU_BAR_HEIGHT 19
	
	#define TIME_BAR_HEIGHT_PROP 0.03f

	#define CONFIG_HEIGHT_PROP 0.67f
	#define CONFIG_WIDTH_PROP 0.25f

	#define SCENE_HEIGHT_PROP 0.67f
	#define SCENE_WIDTH_PROP 0.45f

	#define MODEL_PROPERTIES_HEIGHT_PROP 0.67f
	#define MODEL_PROPERTIES_WIDTH_PROP 0.3f

	#define CONSOLE_HEIGHT_PROP 0.3f

	FileExplorerUI file_explorer_ui;
private:

	bool show_scene_tab = true;
	bool show_game_tab = true;
	bool show_hierarchy_window = true;
	bool show_model_inspector_window = true;
	bool show_configuration_window = false;
	bool show_console_window = true;
	bool show_file_explorer_window = true;
	bool show_debug_window = false;
	bool show_about_window = false;
};

#endif //_ENGINEUI_H_