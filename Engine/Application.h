#pragma once

#include <list>

#include "Globals.h"
#include "Module.h"

class ModuleRender;
class ModuleWindow;
class ModuleTextures;
class ModuleInput;
class ModuleRenderExercise;
class ModuleProgram;
class ModuleTexture;
class ModuleEditor;
class ModuleCamera;
class ModuleModelLoader;

class EngineLog;
class EngineUI;

class TimerUs;

class Application
{
public:

	Application();
	~Application();

	bool Init();
	update_status Update();
	bool CleanUp();

public:
	ModuleRender* renderer = nullptr;
	ModuleWindow* window = nullptr;
	ModuleInput* input = nullptr;
	ModuleRenderExercise* exercise = nullptr;
	ModuleProgram* program = nullptr;
	ModuleTexture* texture = nullptr;
	ModuleEditor* editor = nullptr;
	ModuleCamera* cameras = nullptr;
	ModuleModelLoader* model_loader = nullptr;

	EngineUI* ui = nullptr;
	EngineLog* log = nullptr;

	TimerUs *app_timer = nullptr;

private:

	std::list<Module*> modules;

};

extern Application* App;
