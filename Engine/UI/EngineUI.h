#ifndef _ENGINEUI_H_
#define _ENGINEUI_H_

#include "Main/Application.h"
#include "Main/Globals.h"

class EngineUI
{
public:
	EngineUI() = default;
	~EngineUI() = default;

	void ShowEngineUI();

private:

	void ShowMainMenu();
	void ShowFileMenu();
	void ShowViewMenu();
	void ShowHelpMenu();

	void ShowSceneControls();
	
	void ShowConfigurationWindow();
	void ShowHardware() const;

	void ShowAboutWindow();

	void ShowPopups() const;

	void MenuURL(const std::string& text, const std::string& link);

public:
	#define MAIN_MENU_BAR_HEIGHT 19
	
	#define TIME_BAR_HEIGHT_PROP 0.03f

private:
	bool show_scene_window = true;
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