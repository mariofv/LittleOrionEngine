#pragma once

#include "Globals.h"
#include "imgui.h"
#include <vector>

struct ImGuiTextBuffer;

class EngineLog
{
public:
	EngineLog();
	~EngineLog();

	void logLine(const char* line);
	bool hasPendingData();
	const char* getData() const;

	void logFPS(const float dps);
	const std::vector<float> getFPSData() const;

private:
	ImGuiTextBuffer buffer;

	std::vector<float> fps_log;
};