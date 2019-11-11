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
	void ShowMainMenu();

	void ShowConfigurationWindow();

	void ShowDebugWindow();
	void ShowConsole();
	void ShowPerformanceGraphs();
	void ShowFPSGraph();
	void ShowMSGraph();	
	void ShowHardware();

	void ShowAboutWindow();

private:
	bool show_configuration_window = false;
	bool show_debug_window = false;
	bool show_about_window = false;
};

#endif //_ENGINEUI_H_