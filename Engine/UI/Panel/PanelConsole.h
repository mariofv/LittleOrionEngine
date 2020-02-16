#ifndef _PANELCONSOLE_H_
#define _PANELCONSOLE_H_

#include "Main/Globals.h"
#include "UI/Panel/Panel.h"

#include <imgui.h>

#include <vector>

struct ImGuiTextBuffer;

class PanelConsole : public Panel
{
public:
	PanelConsole();
	~PanelConsole() = default;

	void Render() override;
	void ShowFilterButton(const char* button_label, const ImVec4 & color, bool &filter);

private:
	bool app_filter = false;
	bool assimp_filter = false;
	bool opengl_filter = false;
};

#endif //_PANELCONSOLE_H_