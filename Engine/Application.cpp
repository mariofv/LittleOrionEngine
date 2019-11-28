#include "Application.h"
#include "Module/ModuleWindow.h"
#include "Module/ModuleRender.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleProgram.h"
#include "Module/ModuleTexture.h"
#include "Module/ModuleEditor.h"
#include "Module/ModuleCamera.h"
#include "Module/ModuleModelLoader.h"
#include "Module/ModuleScene.h"
#include "Module/ModuleTime.h"
#include "EngineUI.h"
#include "EngineLog.h"
#include "TimerUs.h"

using namespace std;

Application::Application()
{
	// Order matters: they will Init/start/update in this order
	modules.push_back(window = new ModuleWindow());
	modules.push_back(editor = new ModuleEditor());
	modules.push_back(time = new ModuleTime());
	modules.push_back(renderer = new ModuleRender());
	modules.push_back(scene = new ModuleScene());
	modules.push_back(input = new ModuleInput());
	modules.push_back(program = new ModuleProgram());
	modules.push_back(texture = new ModuleTexture());
	modules.push_back(cameras = new ModuleCamera());
	modules.push_back(model_loader = new ModuleModelLoader());
		
	engine_log = new EngineLog();
	ui = new EngineUI();
}

Application::~Application()
{
	for(list<Module*>::iterator it = modules.begin(); it != modules.end(); ++it)
    {
        delete *it;
    }

	delete ui;
	delete engine_log;
}

bool Application::Init()
{
	bool ret = true;

	for (list<Module*>::iterator it = modules.begin(); it != modules.end() && ret; ++it) 
	{
		ret = (*it)->Init();
	}

	return ret;
}

update_status Application::Update()
{
	update_status ret = update_status::UPDATE_CONTINUE;

	for(list<Module*>::iterator it = modules.begin(); it != modules.end() && ret == update_status::UPDATE_CONTINUE; ++it)
		ret = (*it)->PreUpdate();

	for(list<Module*>::iterator it = modules.begin(); it != modules.end() && ret == update_status::UPDATE_CONTINUE; ++it)
		ret = (*it)->Update();

	for(list<Module*>::iterator it = modules.begin(); it != modules.end() && ret == update_status::UPDATE_CONTINUE; ++it)
		ret = (*it)->PostUpdate();

	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;

	for (list<Module*>::reverse_iterator it = modules.rbegin(); it != modules.rend() && ret; ++it)
		ret = (*it)->CleanUp();

	return ret;
}
