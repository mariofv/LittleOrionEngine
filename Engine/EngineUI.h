#ifndef _ENGINEUI_H_
#define _ENGINEUI_H_

#include "Application.h"
#include "Globals.h"

class EngineUI
{
public:
	EngineUI();
	~EngineUI();

	void ShowEngineUI();
	void InitUI();

private:
	void ShowSceneWindow();

	void ShowMainMenu();
	void ShowFileMenu();
	void ShowViewMenu();
	void ShowHelpMenu();

	void ShowConfigurationWindow();
	void ShowHardware();

	void ShowConsoleWindow();

	void ShowDebugWindow();

	void ShowAboutWindow();

private:
	bool show_scene_window = true;
	bool show_configuration_window = false;
	bool show_console_window = false;
	bool show_debug_window = false;
	bool show_about_window = false;
};

#endif //_ENGINEUI_H_