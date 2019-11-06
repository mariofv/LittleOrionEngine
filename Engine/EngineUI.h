#pragma once

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
	const static void ShowConfigurationWindow();
	const static void ShowWindowOptions();

	const static void ShowDebugWindow();
	const static void ShowConsole();
	const static void ShowFPSGraph();
	const static void ShowMSGraph();	
};

