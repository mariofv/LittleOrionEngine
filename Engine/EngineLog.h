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

	void logFPS(const float fps);
	const std::vector<float> getFPSData() const;

	void logMS(const float ms);
	const std::vector<float> getMSData() const;

private:
	ImGuiTextBuffer buffer;

	std::vector<float> fps_log;
	std::vector<float> ms_log;
};