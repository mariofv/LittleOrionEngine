#include "Globals.h"
#include "Application.h"
#include "UI/EngineLog.h"

void logentry(const EngineLog::LogEntrySource source, const EngineLog::LogEntryType type, const char file[], int line, const char* format, ...)
{
	static char message[4096];
	static char complete_message[4096];
	static va_list ap;

	// Construct the string from variable arguments
	va_start(ap, format);
	vsprintf_s(message, 4096, format, ap);
	va_end(ap);
	sprintf_s(complete_message, 4096, "\n%s(%d) : %s", file, line, message);
	OutputDebugString(complete_message);

	if (App != nullptr) 
	{
		App->engine_log->Log(source, type, file, line, message);
	}
}

