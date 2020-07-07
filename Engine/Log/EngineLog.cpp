#include "EngineLog.h"

#include "Main/Globals.h"

#include <algorithm>
#include <mutex>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/dup_filter_sink.h>
#include <spdlog/sinks/msvc_sink.h>
#include "spdlog/sinks/stdout_color_sinks.h"

EngineLog::EngineLog()
{
	auto visual_studio_debug_sink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
	auto windows_console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
	auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("log.txt", true);

	auto little_orion_duplicate_filter = std::make_shared<spdlog::sinks::dup_filter_sink_st>(std::chrono::seconds(5));
	little_orion_sink = std::make_shared<engine_sink_mt>();
	little_orion_duplicate_filter->add_sink(little_orion_sink);

	auto game_duplicate_filter = std::make_shared<spdlog::sinks::dup_filter_sink_st>(std::chrono::seconds(5));
	game_sink = std::make_shared<engine_sink_mt>();
	game_duplicate_filter->add_sink(game_sink);

	auto opengl_duplicate_filter = std::make_shared<spdlog::sinks::dup_filter_sink_st>(std::chrono::seconds(5));
	opengl_sink = std::make_shared<engine_sink_mt>();
	opengl_duplicate_filter->add_sink(opengl_sink);

	auto assimp_duplicate_filter = std::make_shared<spdlog::sinks::dup_filter_sink_st>(std::chrono::seconds(5));
	assimp_sink = std::make_shared<engine_sink_mt>();
	assimp_duplicate_filter->add_sink(assimp_sink);

	auto resources_duplicate_filter = std::make_shared<spdlog::sinks::dup_filter_sink_st>(std::chrono::seconds(5));
	resources_sink = std::make_shared<engine_sink_mt>();
	resources_duplicate_filter->add_sink(resources_sink);


	std::vector<spdlog::sink_ptr> little_orion_sinks = { little_orion_duplicate_filter, visual_studio_debug_sink, windows_console_sink, file_sink };
	std::vector<spdlog::sink_ptr> game_sinks = { game_duplicate_filter, visual_studio_debug_sink, windows_console_sink, file_sink };
	std::vector<spdlog::sink_ptr> opengl_sinks = { opengl_duplicate_filter, visual_studio_debug_sink, windows_console_sink, file_sink};
	std::vector<spdlog::sink_ptr> assimp_sinks = { assimp_duplicate_filter, visual_studio_debug_sink, windows_console_sink, file_sink};
	std::vector<spdlog::sink_ptr> resources_sinks = { resources_duplicate_filter, visual_studio_debug_sink, windows_console_sink, file_sink };

	little_orion_logger = std::make_shared<spdlog::logger>("LittleOrionEngine", little_orion_sinks.begin(), little_orion_sinks.end());
	game_logger = std::make_shared<spdlog::logger>("Game", game_sinks.begin(), game_sinks.end());
	opengl_logger = std::make_shared<spdlog::logger>("OpenGL", opengl_sinks.begin(), opengl_sinks.end());
	assimp_logger = std::make_shared<spdlog::logger>("Assimp", assimp_sinks.begin(), assimp_sinks.end());
	resources_logger = std::make_shared<spdlog::logger>("Resources", resources_sinks.begin(), resources_sinks.end());
}

EngineLog::~EngineLog()
{
	
}

void EngineLog::Log(const char* message, ...)
{
	va_list ap;
	char complete_message[4096];

	va_start(ap, message);
	vsprintf_s(complete_message, 4096, message, ap);
	va_end(ap);

	game_logger->info(complete_message);
}

void EngineLog::LittleOrionLog(const char* message, const LogEntryType type)
{
	switch (type)
	{
	case LogEntryType::LOG_ERROR:
		little_orion_logger->error(message);
		break;
	case LogEntryType::LOG_INFO:
		little_orion_logger->info(message);
		break;
	case LogEntryType::LOG_SECTION:
		little_orion_logger->trace(message);
		break;
	}
	scroll_down = true;
}

void EngineLog::AssimpLog(const char* message, const LogEntryType type)
{
	switch (type)
	{
	case LogEntryType::LOG_ERROR:
		assimp_logger->error(message);
		break;
	case LogEntryType::LOG_INFO:
		assimp_logger->info(message);
		break;
	case LogEntryType::LOG_DEBUG:
		assimp_logger->debug(message);
		break;
	case LogEntryType::LOG_WARNING:
		assimp_logger->warn(message);
		break;
	}
}

void EngineLog::OpenGLLog(const char* message, const LogEntryType type)
{
	switch (type)
	{
	case LogEntryType::LOG_ERROR:
		opengl_logger->error(message);
		break;
	case LogEntryType::LOG_INFO:
		opengl_logger->info(message);
		break;
	case LogEntryType::LOG_SECTION:
		opengl_logger->trace(message);
		break;
	}
}

void EngineLog::ResourcesLog(const char* message, const LogEntryType type)
{
	switch (type)
	{
	case LogEntryType::LOG_ERROR:
		resources_logger->error(message);
		break;
	case LogEntryType::LOG_INFO:
		resources_logger->info(message);
		break;
	case LogEntryType::LOG_WARNING:
		resources_logger->trace(message);
		break;
	}
}