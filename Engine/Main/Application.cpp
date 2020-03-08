#include "Application.h"
#include "Module/ModuleActions.h"
#include "Module/ModuleCamera.h"
#include "Module/ModuleDebug.h"
#include "Module/ModuleDebugDraw.h"
#include "Module/ModuleEditor.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleLight.h"
#include "Module/ModuleModelLoader.h"
#include "Module/ModuleResourceManager.h"
#include "Module/ModuleProgram.h"
#include "Module/ModuleRender.h"
#include "Module/ModuleScene.h"
#include "Module/ModuleTexture.h"
#include "Module/ModuleTime.h"
#include "Module/ModuleUI.h"
#include "Module/ModuleFileSystem.h"
#include "Module/ModuleWindow.h"
#include "UI/EngineLog.h"
#include "Brofiler/Brofiler.h"


Application::Application()
{
	modules.reserve(15);
	// Order matters: they will Init/start/update in this order
	modules.emplace_back(window = new ModuleWindow());
	modules.emplace_back(filesystem = new ModuleFileSystem());
	modules.emplace_back(resources = new ModuleResourceManager());
	modules.emplace_back(input = new ModuleInput());
	modules.emplace_back(ui = new ModuleUI());
	modules.emplace_back(time = new ModuleTime());
	modules.emplace_back(texture = new ModuleTexture());
	modules.emplace_back(renderer = new ModuleRender());
	modules.emplace_back(editor = new ModuleEditor());
	modules.emplace_back(actions = new ModuleActions());
	modules.emplace_back(program = new ModuleProgram());
	modules.emplace_back(cameras = new ModuleCamera());
	modules.emplace_back(model_loader = new ModuleModelLoader());
	modules.emplace_back(debug = new ModuleDebug());
	modules.emplace_back(debug_draw = new ModuleDebugDraw());
	modules.emplace_back(lights = new ModuleLight());
	modules.emplace_back(scene = new ModuleScene());
		
	engine_log = std::make_unique<EngineLog>();
}

Application::~Application()
{
	for(std::vector<Module*>::iterator it = modules.begin(); it != modules.end(); ++it)
    {
        delete *it;
    }
}

bool Application::Init()
{
	bool result = true;

	for (auto &module : modules) 
	{
		bool ret = module->Init();
		if (!ret) {
			result = ret;
		}
	}
	return result;
}

update_status Application::Update()
{
	BROFILER_FRAME("MainLoop");
	update_status result = update_status::UPDATE_CONTINUE;
	for (auto &module : modules) 
	{
		update_status ret = module->PreUpdate();
		if (ret == update_status::UPDATE_ERROR || ret == update_status::UPDATE_STOP) 
		{
			result = ret;
		}
	}
	if (result == update_status::UPDATE_CONTINUE) 
	{
		for (auto &module : modules) 
		{
			update_status ret = module->Update();
			if (ret == update_status::UPDATE_ERROR || ret == update_status::UPDATE_STOP) 
			{
				result = ret;
			}
		}
	}

	if (result == update_status::UPDATE_CONTINUE) 
	{
		for (auto &module : modules) 
		{
			update_status ret = module->PostUpdate();
			if (ret == update_status::UPDATE_ERROR || ret == update_status::UPDATE_STOP) 
			{
				result = ret;
			}
		}
	}

	if (result == update_status::UPDATE_CONTINUE)
	{
		App->renderer->Render();
	}

	return result;
}

bool Application::CleanUp()
{
	bool ret = true;

	for (std::vector<Module*>::reverse_iterator it = modules.rbegin(); it != modules.rend() && ret; ++it)
		ret = (*it)->CleanUp();

	return ret;
}
