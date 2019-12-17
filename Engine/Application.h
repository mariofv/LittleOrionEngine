#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include "Globals.h"
#include "Module/Module.h"

#include <vector>

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
class ModuleTime;
class ModuleScene;
class ModuleFileSystem;
class ModuleDebug;

class EngineLog;
class EngineUI;


class SceneImporter;
class MaterialImporter;

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
	ModuleProgram* program = nullptr;
	ModuleTexture* texture = nullptr;
	ModuleEditor* editor = nullptr;
	ModuleCamera* cameras = nullptr;
	ModuleModelLoader* model_loader = nullptr;
	ModuleTime* time = nullptr;
	ModuleScene* scene = nullptr;
	ModuleFileSystem* filesystem = nullptr;
	ModuleDebug* debug = nullptr;

	EngineUI* ui = nullptr;
	EngineLog* engine_log = nullptr;
	SceneImporter* scene_importer = nullptr;
	MaterialImporter* material_importer = nullptr;

private:

	std::vector<Module*> modules;

};

extern Application* App;

#endif // _APPLICATION_H_