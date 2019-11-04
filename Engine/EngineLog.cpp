#include "Globals.h"
#include "EngineLog.h"
#include "imgui.h"

EngineLog::EngineLog()
{
}

EngineLog::~EngineLog()
{
	buffer.clear();
}


void EngineLog::log(const char* line)
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