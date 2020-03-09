#include "Main/Globals.h"
#include "EngineLog.h"

#include <algorithm>


EngineLog::~EngineLog()
{
	for (unsigned int i = 0; i < text_log.size(); ++i)
	{
		delete text_log[i];
	}

	text_log.clear();
	fps_log.clear();
	ms_log.clear();
}


void EngineLog::Log(const LogEntrySource source, const LogEntryType type, const char* file, int line, const char* message)
{
	LogEntry *new_log_entry = new LogEntry(source, type, file, line, message);
	text_log.push_back(new_log_entry);
	scroll_down = true;
}

void EngineLog::LogFPS(float fps)
{
	if (fps_log.size() < 100)
	{
		fps_log.push_back(fps);
	}
	else 
	{
		rotate(fps_log.begin(), fps_log.begin() + 1, fps_log.end());
		fps_log[99] = fps;
	}
}

void EngineLog::LogMS(float ms)
{
	if (ms_log.size() < 100)
	{
		ms_log.push_back(ms);
	}
	else
	{
		rotate(ms_log.begin(), ms_log.begin() + 1, ms_log.end());
		ms_log[99] = ms;
	}
}

std::vector<float> EngineLog::getFPSData() const
{
	return fps_log;
}

std::vector<float> EngineLog::getMSData() const
{
	return ms_log;
}