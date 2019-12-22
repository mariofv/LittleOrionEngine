#include "Application.h"
#include "Module/ModuleCamera.h"
#include "Module/ModuleDebug.h"
#include "Module/ModuleEditor.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleModelLoader.h"
#include "Module/ModuleProgram.h"
#include "Module/ModuleRender.h"
#include "Module/ModuleScene.h"
#include "Module/ModuleTexture.h"
#include "Module/ModuleTime.h"
#include "Module/ModuleUI.h"
#include "Module/ModuleFileSystem.h"
#include "Module/ModuleWindow.h"
#include "Importer/MaterialImporter.h"
#include "Importer/MeshImporter.h"
#include "UI/EngineUI.h"
#include "UI/EngineLog.h"
#include "TimerUs.h"
#include "Brofiler/Brofiler.h"

#include <unordered_map>

using namespace std;

Application::Application()
{
	modules.reserve(10);
	// Order matters: they will Init/start/update in this order
	modules.emplace_back(window = new ModuleWindow());
	modules.emplace_back(filesystem = new ModuleFileSystem());
	modules.emplace_back(editor = new ModuleEditor());
	modules.emplace_back(ui = new ModuleUI());
	modules.emplace_back(time = new ModuleTime());
	modules.emplace_back(texture = new ModuleTexture());
	modules.emplace_back(renderer = new ModuleRender());
	modules.emplace_back(scene = new ModuleScene());
	modules.emplace_back(input = new ModuleInput());
	modules.emplace_back(program = new ModuleProgram());
	modules.emplace_back(cameras = new ModuleCamera());
	modules.emplace_back(model_loader = new ModuleModelLoader());
	modules.emplace_back(debug = new ModuleDebug());
		
	engine_log = new EngineLog();

	material_importer = new MaterialImporter();
	mesh_importer = new MeshImporter();
}

Application::~Application()
{
	for(vector<Module*>::iterator it = modules.begin(); it != modules.end(); ++it)
    {
        delete *it;
    }

	delete engine_log;
	delete material_importer;
	delete mesh_importer;
}

bool Application::Init()
{
	bool ret = true;

	for (vector<Module*>::iterator it = modules.begin(); it != modules.end() && ret; ++it) 
	{
		ret = (*it)->Init();
	}

	return ret;
}

update_status Application::Update()
{
	BROFILER_FRAME("MainLoop");
	update_status ret = update_status::UPDATE_CONTINUE;

	for(vector<Module*>::iterator it = modules.begin(); it != modules.end() && ret == update_status::UPDATE_CONTINUE; ++it)
		ret = (*it)->PreUpdate();

	for(vector<Module*>::iterator it = modules.begin(); it != modules.end() && ret == update_status::UPDATE_CONTINUE; ++it)
		ret = (*it)->Update();

	for(vector<Module*>::iterator it = modules.begin(); it != modules.end() && ret == update_status::UPDATE_CONTINUE; ++it)
		ret = (*it)->PostUpdate();

	App->renderer->Render();

	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;

	for (vector<Module*>::reverse_iterator it = modules.rbegin(); it != modules.rend() && ret; ++it)
		ret = (*it)->CleanUp();

	return ret;
}
