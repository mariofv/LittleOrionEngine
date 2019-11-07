#include "Globals.h"
#include "EngineLog.h"
#include "imgui.h"
#include <algorithm>

EngineLog::EngineLog()
{
}

EngineLog::~EngineLog()
{
	buffer.clear();
	fps_log.clear();
}


void EngineLog::logLine(const char* line)
{
	buffer.append(line);
}

bool EngineLog::hasPendingData() 
{
	return !buffer.empty();
}

const char* EngineLog::getData() const
{
	return buffer.begin();
}

void EngineLog::logFPS(const float fps)
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

void EngineLog::logMS(const float ms)
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

const std::vector<float> EngineLog::getFPSData() const
{
	return fps_log;
}

const std::vector<float> EngineLog::getMSData() const
{
	return ms_log;
}
