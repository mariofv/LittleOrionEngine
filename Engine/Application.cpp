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
#include "UI/EngineUI.h"
#include "UI/EngineLog.h"
#include "TimerUs.h"

using namespace std;

Application::Application()
{
	modules.reserve(10);
	// Order matters: they will Init/start/update in this order
	modules.emplace_back(window = new ModuleWindow());
	modules.emplace_back(editor = new ModuleEditor());
	modules.emplace_back(time = new ModuleTime());
	modules.emplace_back(renderer = new ModuleRender());
	modules.emplace_back(scene = new ModuleScene());
	modules.emplace_back(input = new ModuleInput());
	modules.emplace_back(program = new ModuleProgram());
	modules.emplace_back(texture = new ModuleTexture());
	modules.emplace_back(cameras = new ModuleCamera());
	modules.emplace_back(model_loader = new ModuleModelLoader());
		
	engine_log = new EngineLog();
	ui = new EngineUI();
}

Application::~Application()
{
	for(vector<Module*>::iterator it = modules.begin(); it != modules.end(); ++it)
    {
        delete *it;
    }

	delete ui;
	delete engine_log;
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
	update_status ret = update_status::UPDATE_CONTINUE;

	for(vector<Module*>::iterator it = modules.begin(); it != modules.end() && ret == update_status::UPDATE_CONTINUE; ++it)
		ret = (*it)->PreUpdate();

	for(vector<Module*>::iterator it = modules.begin(); it != modules.end() && ret == update_status::UPDATE_CONTINUE; ++it)
		ret = (*it)->Update();

	for(vector<Module*>::iterator it = modules.begin(); it != modules.end() && ret == update_status::UPDATE_CONTINUE; ++it)
		ret = (*it)->PostUpdate();

	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;

	for (vector<Module*>::reverse_iterator it = modules.rbegin(); it != modules.rend() && ret; ++it)
		ret = (*it)->CleanUp();

	return ret;
}
