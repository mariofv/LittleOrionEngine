#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include "Globals.h"
#include "Module/Module.h"

#include <vector>
#include <memory>

class EngineLog;

class EventManager;

class ModuleActions;
class ModuleAI;
class ModuleAnimation;
class ModuleAudio;
class ModuleCamera;
class ModuleDebug;
class ModuleDebugDraw;
class ModuleEditor;
class ModuleEffects;
class ModuleFileSystem;
class ModuleInput;
class ModuleLight;
class ModulePhysics;
class ModuleProgram;
class ModuleRender;
class ModuleResourceManager;
class ModuleScene;
class ModuleScriptManager;
class ModuleSpacePartitioning;
class ModuleTexture;
class ModuleTime;
class ModuleUI;
class ModuleWindow;

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

	ModuleActions* actions = nullptr;
	ModuleAI* artificial_intelligence = nullptr;
	ModuleAnimation* animations = nullptr;
	ModuleAudio* audio = nullptr;
	ModuleCamera* cameras = nullptr;
	ModuleDebug* debug = nullptr;
	ModuleDebugDraw* debug_draw = nullptr;
	ModuleEditor* editor = nullptr;
	ModuleEffects* effects = nullptr;
	ModuleFileSystem* filesystem = nullptr;
	ModuleInput* input = nullptr;
	ModuleLight* lights = nullptr;
	ModulePhysics* physics = nullptr;
	ModuleProgram* program = nullptr;
	ModuleRender* renderer = nullptr;
	ModuleResourceManager* resources = nullptr;
	ModuleScene* scene = nullptr;
	ModuleScriptManager* scripts = nullptr;
	ModuleSpacePartitioning* space_partitioning = nullptr;
	ModuleTexture* texture = nullptr;
	ModuleTime* time = nullptr;
	ModuleUI* ui = nullptr;
	ModuleWindow* window = nullptr;

	std::unique_ptr<EngineLog> engine_log = nullptr;
	std::unique_ptr<EventManager> event_manager = nullptr;

private:
	std::vector<Module*> modules;
};

extern Application* App;

#endif // _APPLICATION_H_