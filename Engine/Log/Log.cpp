#include "Main/Globals.h"
#include "Main/Application.h"
#include "Filesystem/Path.h"
#include "Log/EngineLog.h"
#include <mutex>

void LittleOrionLogEntry(const EngineLog::LogEntryType type, const char file[], int line, const char* format, ...)
{
	static char message[4096];
	static char complete_message[4096];
	static va_list ap;

	// Construct the string from variable arguments
	va_start(ap, format);
	vsprintf_s(message, 4096, format, ap);
	va_end(ap);
	std::string filename = Path::GetFilenameWindows(std::string(file));
	sprintf_s(complete_message, 4096, "%s(%d) : %s", filename.c_str(), line, message);
	
	App->engine_log->LittleOrionLog(complete_message, type);
}

void OpenGLLogEntry(const EngineLog::LogEntryType type, const char file[], const int line, const char* format, ...)
{
	static char message[4096];
	static char complete_message[4096];
	static va_list ap;

	// Construct the string from variable arguments
	va_start(ap, format);
	vsprintf_s(message, 4096, format, ap);
	va_end(ap);
	std::string filename = Path::GetFilenameWindows(std::string(file));
	sprintf_s(complete_message, 4096, "%s(%d) : %s", filename.c_str(), line, message);

	App->engine_log->OpenGLLog(complete_message, type);
}

void AssimpLogEntry(const EngineLog::LogEntryType type, const char file[], const int line, const char* format, ...)
{
	static char message[4096];
	static char complete_message[4096];
	static va_list ap;

	// Construct the string from variable arguments
	va_start(ap, format);
	vsprintf_s(message, 4096, format, ap);
	va_end(ap);
	std::string filename = Path::GetFilenameWindows(std::string(file));
	sprintf_s(complete_message, 4096, "%s(%d) : %s", filename.c_str(), line, message);

	App->engine_log->AssimpLog(complete_message, type);
}

void ResourceLogEntry(const EngineLog::LogEntryType type, const char file[], const int line, const char* format, ...)
{
	static char message[4096];
	static char complete_message[4096];
	static va_list ap;

	// Construct the string from variable arguments
	va_start(ap, format);
	vsprintf_s(message, 4096, format, ap);
	va_end(ap);
	std::string filename = Path::GetFilenameWindows(std::string(file));
	sprintf_s(complete_message, 4096, "%s(%d) : %s", filename.c_str(), line, message);

	App->engine_log->ResourcesLog(complete_message, type);
}

void DebugLogEntry(const char file[], const int line, const char* format, ...)
{
	static char message[4096];
	static char complete_message[4096];
	static va_list ap;

	// Construct the string from variable arguments
	va_start(ap, format);
	vsprintf_s(message, 4096, format, ap);
	va_end(ap);
	std::string filename = Path::GetFilenameWindows(std::string(file));
	sprintf_s(complete_message, 4096, "%s(%d) : %s", filename.c_str(), line, message);

	App->engine_log->DebugLog(complete_message);
}