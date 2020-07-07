#ifndef _PANELCONSOLE_H_
#define _PANELCONSOLE_H_

#include "EditorUI/Panel/Panel.h"
#include "Main/Globals.h"

#include <imgui.h>
#include <vector>

struct ImGuiTextBuffer;

class PanelConsole : public Panel
{
public:
	enum class CurrentLog
	{
		GAME,
		LITTLEORION,
		ASSIMP,
		OPENGL,
		RESOURCES
	};

	PanelConsole();
	~PanelConsole() = default;

	void Render() override;

private:
	void SetCurrentLog(CurrentLog current_log);
	void ClearCurrentLog();

private:
	std::shared_ptr<spdlog::logger> current_log = nullptr;
	CurrentLog current_log_type = CurrentLog::LITTLEORION;
	std::string current_log_name = "LittleOrion";
};

#endif //_PANELCONSOLE_H_