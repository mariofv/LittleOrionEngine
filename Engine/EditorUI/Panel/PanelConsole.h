#ifndef _PANELCONSOLE_H_
#define _PANELCONSOLE_H_

#include "EditorUI/Panel/Panel.h"
#include "Main/Globals.h"
#include "Log/EngineLog.h"

#include <imgui.h>
#include <vector>

struct ImGuiTextBuffer;

class PanelConsole : public Panel
{
public:
	enum class CurrentLog
	{
		GAMELOG,
		LITTLEORION,
		ASSIMP,
		OPENGL,
		RESOURCES,
		DEBUGLOG
	};

	PanelConsole();
	~PanelConsole() = default;

	void Render() override;

	void ClearGameLog();

private:
	void SetCurrentLog(CurrentLog current_log);
	void ClearCurrentLog();

public:
	bool clear_on_play = true;

private:
	std::shared_ptr<engine_sink_mt> current_sink = nullptr;
	CurrentLog current_log_type = CurrentLog::LITTLEORION;
	std::string current_log_name = "LittleOrion";

};

#endif //_PANELCONSOLE_H_