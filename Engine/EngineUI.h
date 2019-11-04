#pragma once

class EngineUI
{
public:
	EngineUI();
	~EngineUI();

	const static void ShowEngineUI();

private:
	const static void ShowLog();

	const static void ShowAppInfo();
	const static void ShowFPSGraph();
};

