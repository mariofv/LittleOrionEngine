#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include "Globals.h"
#include "Module/Module.h"

#include <vector>
#include <memory>

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
class ModuleDebugDraw;
class ModuleUI;
class ModuleLight;

class EngineLog;


class MaterialImporter;
class MeshImporter;

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
	ModuleDebugDraw* debug_draw = nullptr;
	ModuleUI* ui = nullptr;
	ModuleLight* lights = nullptr;

	std::unique_ptr<EngineLog> engine_log = nullptr;
	std::unique_ptr<MeshImporter> mesh_importer = nullptr;
	std::unique_ptr<MaterialImporter> material_importer = nullptr;

private:

	std::vector<Module*> modules;

};

extern Application* App;

#endif // _APPLICATION_H_