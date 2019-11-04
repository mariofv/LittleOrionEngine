#pragma once

class EngineUI
{
public:
	EngineUI();
	~EngineUI();

	const static void ShowEngineUI();

private:
	const static void ShowConfigurationWindow();
	const static void ShowWindowOptions();

	const static void ShowDebugWindow();
	const static void ShowConsole();
	const static void ShowFPSGraph();
};

