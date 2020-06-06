#include "Application.h"
#include "EditorUI/EngineLog.h"
#include "Module/ModuleActions.h"
#include "Module/ModuleAI.h"
#include "Module/ModuleAudio.h"
#include "Module/ModuleAnimation.h"
#include "Module/ModuleCamera.h"
#include "Module/ModuleDebug.h"
#include "Module/ModuleDebugDraw.h"
#include "Module/ModuleEditor.h"
#include "Module/ModuleFileSystem.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleLight.h"
#include "Module/ModuleProgram.h"
#include "Module/ModulePhysics.h"
#include "Module/ModuleRender.h"
#include "Module/ModuleResourceManager.h"
#include "Module/ModuleScene.h"
#include "Module/ModuleScriptManager.h"
#include "Module/ModuleSpacePartitioning.h"
#include "Module/ModuleTexture.h"
#include "Module/ModuleTime.h"
#include "Module/ModuleUI.h"
#include "Module/ModuleWindow.h"

#include <Brofiler/Brofiler.h>


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
	modules.emplace_back(animations = new ModuleAnimation());
	modules.emplace_back(editor = new ModuleEditor());
	modules.emplace_back(actions = new ModuleActions());
	modules.emplace_back(program = new ModuleProgram());
	modules.emplace_back(cameras = new ModuleCamera());
	modules.emplace_back(debug = new ModuleDebug());
	modules.emplace_back(audio = new ModuleAudio());
	modules.emplace_back(lights = new ModuleLight());
	modules.emplace_back(scene = new ModuleScene());
	modules.emplace_back(space_partitioning = new ModuleSpacePartitioning());
	modules.emplace_back(artificial_intelligence = new ModuleAI());
	modules.emplace_back(physics = new ModulePhysics());
	modules.emplace_back(scripts = new ModuleScriptManager());
	modules.emplace_back(debug_draw = new ModuleDebugDraw());
		
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

	for (auto& module : modules) 
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

	if (App->scene->HasPendingSceneToLoad())
	{
		App->scene->OpenPendingScene();
		App->physics->UpdateAllDimensions();
	}

	for (auto& module : modules) 
	{
		update_status ret = module->PreUpdate();
		if (ret == update_status::UPDATE_ERROR || ret == update_status::UPDATE_STOP) 
		{
			result = ret;
		}
	}
	if (result == update_status::UPDATE_CONTINUE) 
	{
		for (auto& module : modules) 
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
		for (auto& module : modules) 
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

	App->time->EndFrame();

	return result;
}

bool Application::CleanUp()
{
	bool ret = true;

	for (std::vector<Module*>::reverse_iterator it = modules.rbegin(); it != modules.rend() && ret; ++it)
		ret = (*it)->CleanUp();

	return ret;
}
