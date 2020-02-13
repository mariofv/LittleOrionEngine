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
	void ShowSceneControls();

	void ShowAboutWindow();

	void ShowPopups() const;

	void MenuURL(const std::string& text, const std::string& link);

public:
	#define MAIN_MENU_BAR_HEIGHT 19
	
	#define TIME_BAR_HEIGHT_PROP 0.03f
};

#endif //_ENGINEUI_H_