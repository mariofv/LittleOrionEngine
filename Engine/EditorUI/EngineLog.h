#ifndef _ENGINELOG_H_
#define _ENGINELOG_H_

#include "Main/Globals.h"

#include <imgui.h>

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

#define APP_LOG_SECTION(format, ...) logentry(EngineLog::LogEntrySource::APP_LOG, EngineLog::LogEntryType::LOG_SECTION, __FILE__, __LINE__, format, __VA_ARGS__);
#define APP_LOG_INFO(format, ...) logentry(EngineLog::LogEntrySource::APP_LOG, EngineLog::LogEntryType::LOG_INFO, __FILE__, __LINE__, format, __VA_ARGS__);
#define APP_LOG_INIT(format, ...) logentry(EngineLog::LogEntrySource::APP_LOG, EngineLog::LogEntryType::LOG_INIT, __FILE__, __LINE__, format, __VA_ARGS__);
#define APP_LOG_SUCCESS(format, ...) logentry(EngineLog::LogEntrySource::APP_LOG, EngineLog::LogEntryType::LOG_SUCCESS, __FILE__, __LINE__, format, __VA_ARGS__);
#define APP_LOG_ERROR(format, ...) logentry(EngineLog::LogEntrySource::APP_LOG, EngineLog::LogEntryType::LOG_ERROR, __FILE__, __LINE__, format, __VA_ARGS__);

#define OPENGL_LOG_INFO(format, ...) logentry(EngineLog::LogEntrySource::OPENGL_LOG, EngineLog::LogEntryType::LOG_INFO, __FILE__, __LINE__, format, __VA_ARGS__);
#define OPENGL_LOG_INIT(format, ...) logentry(EngineLog::LogEntrySource::OPENGL_LOG, EngineLog::LogEntryType::LOG_INIT, __FILE__, __LINE__, format, __VA_ARGS__);
#define OPENGL_LOG_SUCCESS(format, ...) logentry(EngineLog::LogEntrySource::OPENGL_LOG, EngineLog::LogEntryType::LOG_SUCCESS, __FILE__, __LINE__, format, __VA_ARGS__);
#define OPENGL_LOG_ERROR(format, ...) logentry(EngineLog::LogEntrySource::OPENGL_LOG, EngineLog::LogEntryType::LOG_ERROR, __FILE__, __LINE__, format, __VA_ARGS__);

#define MYASSIMP_LOG_INFO(format, ...) logentry(EngineLog::LogEntrySource::ASSIMP_LOG, EngineLog::LogEntryType::LOG_INFO, __FILE__, __LINE__, format, __VA_ARGS__);
#define MYASSIMP_LOG_INIT(format, ...) logentry(EngineLog::LogEntrySource::ASSIMP_LOG, EngineLog::LogEntryType::LOG_INIT, __FILE__, __LINE__, format, __VA_ARGS__);
#define MYASSIMP_LOG_SUCCESS(format, ...) logentry(EngineLog::LogEntrySource::ASSIMP_LOG, EngineLog::LogEntryType::LOG_SUCCESS, __FILE__, __LINE__, format, __VA_ARGS__);
#define MYASSIMP_LOG_ERROR(format, ...) logentry(EngineLog::LogEntrySource::ASSIMP_LOG, EngineLog::LogEntryType::LOG_ERROR, __FILE__, __LINE__, format, __VA_ARGS__);

void logentry(const EngineLog::LogEntrySource source, const EngineLog::LogEntryType type, const char file[], const int line, const char* format, ...);

#endif //_ENGINELOG_H_

