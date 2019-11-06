#pragma once

//TODO: Cambiar de static a no static
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

namespace camera_options {
	static float fov;
	static float aspect_ratio;

	static float near_plane;
	static float far_plane;
}

class EngineUI
{
public:
	EngineUI();
	~EngineUI();

	static void ShowEngineUI();
	static void InitUI();

private:
	static void ShowMainMenu();

	static void ShowConfigurationWindow();
	static void ShowWindowOptions();
	static void ShowCameraOptions();

	static void ShowDebugWindow();
	static void ShowConsole();
	static void ShowPerformanceGraphs();
	static void ShowFPSGraph();
	static void ShowMSGraph();	
	static void ShowHardware();

	static void ShowAboutWindow();
};

