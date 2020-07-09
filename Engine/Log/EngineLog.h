#ifndef _ENGINELOG_H_
#define _ENGINELOG_H_

#define ENGINE_EXPORTS

#include "Log/EngineSink.h"
#include "Main/Globals.h"

#include <imgui.h>
#include <spdlog/spdlog.h>
#include <vector>

struct ImGuiTextBuffer;
using engine_sink_mt = EngineSink<std::mutex>;


class EngineLog
{
public:
	enum class LogEntryType
	{
		LOG_SECTION,
		LOG_INFO,
		LOG_ERROR,
		LOG_WARNING,
		LOG_DEBUG
	};

	EngineLog();
	~EngineLog();

	ENGINE_API void Log(const char* message, ...);

	void LittleOrionLog(const char* message, const LogEntryType type);
	void AssimpLog(const char* message, const LogEntryType type);
	void OpenGLLog(const char* message, const LogEntryType type);
	void ResourcesLog(const char* message, const LogEntryType type);
	void DebugLog(const char* message);
	
private:
	std::shared_ptr<spdlog::logger> little_orion_logger = nullptr;
	std::shared_ptr<engine_sink_mt> little_orion_sink = nullptr;

	std::shared_ptr<spdlog::logger> game_logger = nullptr;
	std::shared_ptr<engine_sink_mt> game_sink = nullptr;

	std::shared_ptr<spdlog::logger> opengl_logger = nullptr;
	std::shared_ptr<engine_sink_mt> opengl_sink = nullptr;

	std::shared_ptr<spdlog::logger> assimp_logger = nullptr;
	std::shared_ptr<engine_sink_mt> assimp_sink = nullptr;

	std::shared_ptr<spdlog::logger> resources_logger = nullptr;
	std::shared_ptr<engine_sink_mt> resources_sink = nullptr;

	std::shared_ptr<spdlog::logger> debug_logger = nullptr;
	std::shared_ptr<engine_sink_mt> debug_sink = nullptr;

	bool scroll_down = false;

	friend class PanelConsole;
};

#define APP_LOG_SECTION(format, ...) LittleOrionLogEntry(EngineLog::LogEntryType::LOG_SECTION, __FILE__, __LINE__, format, __VA_ARGS__);
#define APP_LOG_INFO(format, ...) LittleOrionLogEntry(EngineLog::LogEntryType::LOG_INFO, __FILE__, __LINE__, format, __VA_ARGS__);
#define APP_LOG_ERROR(format, ...) LittleOrionLogEntry(EngineLog::LogEntryType::LOG_ERROR, __FILE__, __LINE__, format, __VA_ARGS__);

#define OPENGL_LOG_INFO(format, ...) OpenGLLogEntry(EngineLog::LogEntryType::LOG_INFO, __FILE__, __LINE__, format, __VA_ARGS__);
#define OPENGL_LOG_ERROR(format, ...) OpenGLLogEntry(EngineLog::LogEntryType::LOG_ERROR, __FILE__, __LINE__, format, __VA_ARGS__);

#define MYASSIMP_LOG_INFO(format, ...) AssimpLogEntry(EngineLog::LogEntryType::LOG_INFO, __FILE__, __LINE__, format, __VA_ARGS__);
#define MYASSIMP_LOG_ERROR(format, ...) AssimpLogEntry(EngineLog::LogEntryType::LOG_ERROR, __FILE__, __LINE__, format, __VA_ARGS__);
#define MYASSIMP_LOG_WARNING(format, ...) AssimpLogEntry(EngineLog::LogEntryType::LOG_WARNING, __FILE__, __LINE__, format, __VA_ARGS__);
#define MYASSIMP_LOG_DEBUG(format, ...) AssimpLogEntry(EngineLog::LogEntryType::LOG_DEBUG, __FILE__, __LINE__, format, __VA_ARGS__);

#define RESOURCES_LOG_INFO(format, ...) ResourceLogEntry(EngineLog::LogEntryType::LOG_INFO, __FILE__, __LINE__, format, __VA_ARGS__);
#define RESOURCES_LOG_ERROR(format, ...) ResourceLogEntry(EngineLog::LogEntryType::LOG_ERROR, __FILE__, __LINE__, format, __VA_ARGS__);
#define RESOURCES_LOG_WARNING(format, ...) ResourceLogEntry(EngineLog::LogEntryType::LOG_WARNING, __FILE__, __LINE__, format, __VA_ARGS__);

#define DEBUG_LOG(format, ...) DebugLogEntry(__FILE__, __LINE__, format, __VA_ARGS__);

void LittleOrionLogEntry(const EngineLog::LogEntryType type, const char file[], const int line, const char* format, ...);
void OpenGLLogEntry(const EngineLog::LogEntryType type, const char file[], const int line, const char* format, ...);
void AssimpLogEntry(const EngineLog::LogEntryType type, const char file[], const int line, const char* format, ...);
void ResourceLogEntry(const EngineLog::LogEntryType type, const char file[], const int line, const char* format, ...);
void DebugLogEntry(const char file[], const int line, const char* format, ...);

#endif //_ENGINELOG_H_

