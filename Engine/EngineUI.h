#ifndef _ENGINEUI_H_
#define _ENGINEUI_H_

#include "Application.h"
#include "Globals.h"

//TODO: Cambiar de static a no static
namespace ui_flags {
	
}

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
	void ShowWindowOptions();
	void ShowCameraOptions();

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

	int fullscreen;
	bool bordered;
	bool resizable;

	int height;
	int width;

	float brightness;

	float fov;
	float aspect_ratio;

	float near_plane;
	float far_plane;
};

#endif //_ENGINEUI_H_