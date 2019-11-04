#pragma once
#include "Globals.h"
#include "imgui.h"

struct ImGuiTextBuffer;

class EngineLog
{
public:
	EngineLog();
	~EngineLog();

	void log(const char* line);
	bool hasPendingData();
	const char* getData() const;

private:
	ImGuiTextBuffer buffer;
};