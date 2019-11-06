#pragma once

namespace ui_flags {
	static bool show_configuration_window;
	static bool show_debug_window;
	static bool show_about_window;
}

namespace window_options {
	static int fullscreen;
	static bool bordered;
	static bool resizable;

	static int height;
	static int width;

	static float brightness;
}

class EngineUI
{
public:
	EngineUI();
	~EngineUI();

	const static void ShowEngineUI();
	const static void InitUI();

private:
	const static void ShowMainMenu();

	const static void ShowConfigurationWindow();
	const static void ShowWindowOptions();

	const static void ShowDebugWindow();
	const static void ShowConsole();
	const static void ShowFPSGraph();
	const static void ShowMSGraph();	

	const static void ShowAboutWindow();
};

