#ifndef _ENGINELOG_H_
#define _ENGINELOG_H_

#include "Main/Globals.h"

#include "imgui.h"

#include <vector>

struct ImGuiTextBuffer;

class EngineLog
{
public:
	enum class LogEntryType
	{
		LOG_SECTION,
		LOG_INFO,
		LOG_INIT,
		LOG_SUCCESS,
		LOG_ERROR
	};

	enum class LogEntrySource
	{
		APP_LOG,
		OPENGL_LOG,
		ASSIMP_LOG
	};

	struct LogEntry 
	{
		LogEntry() = default;
		LogEntry(const LogEntrySource source, const LogEntryType type, const char *file, int line, const char *message) :
			source(source),
			type(type),
			file(file),
			line(line),
			message(std::string(message))
		{}

		LogEntryType type = LogEntryType::LOG_ERROR;
		LogEntrySource source = LogEntrySource::APP_LOG;
		const char *file = nullptr;
		int line = 0;
		std::string message = "";
	};


	EngineLog() = default;
	~EngineLog();

	void Log(const LogEntrySource source, const LogEntryType type, const char* file, int line, const char* message);
	void LogFPS(float fps);
	std::vector<float> getFPSData() const;

	void LogMS(float ms);
	std::vector<float> getMSData() const;
	
private:
	std::vector<LogEntry*> text_log;

	std::vector<float> fps_log;
	std::vector<float> ms_log;

	bool scroll_down = false;

	friend class PanelConsole;
};

#endif //_ENGINELOG_H_