#include "EventManager.h"
#include "Component/ComponentScript.h"
#include "Component/ComponentTransform.h"
#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleScene.h"

#include "EditorUI/Panel/InspectorSubpanel/PanelComponent.h"

#include "imgui.h"


EventManager* EventManagerDLL()
{
	EventManager* instance = new EventManager();
	return instance;
}

EventManager::EventManager()
{
	panel = new PanelComponent();
}

// Use this for initialization before Start()
void EventManager::Awake()
{
	GameObject* enemy_manager_go = App->scene->GetGameObjectByName("EnemyManager");
	ComponentScript* enemy_manager_component = enemy_manager_go->GetComponentScript("EnemyManager");
	enemy_manager = static_cast<EnemyManager*>(enemy_manager_component->script);
}

// Use this for initialization
void EventManager::Start()
{

}

// Update is called once per frame
void EventManager::Update()
{	//For now the only event we have is enemy spawning
	if(event_triggered)
	{
		if(enemies_killed_on_wave >= enemies_per_wave)
		{
			enemies_killed_on_wave = 0;
			--waves_left;
			if(waves_left == 0)
			{
				event_triggered = false;
				waves_left = 0;
			}
			else
			{
				enemy_manager->SpawnWave(current_event, enemies_per_wave);
			}
		}

	}

}
// Use this for showing variables on inspector
void EventManager::OnInspector(ImGuiContext* context)
{
	//Necessary to be able to write with imgui
	ImGui::SetCurrentContext(context);
	ShowDraggedObjects();
	ImGui::Text("enemies_killed_on_wave: %d", enemies_killed_on_wave);
	ImGui::Text("enemies_per_wave: %d", enemies_per_wave);
}

//Use this for linking JUST GO automatically 
void EventManager::InitPublicGameObjects()
{
	//IMPORTANT, public gameobjects, name_gameobjects and go_uuids MUST have same size
	for (int i = 0; i < public_gameobjects.size(); ++i)
	{
		name_gameobjects.push_back(is_object);
		go_uuids.push_back(0);
	}
}

bool EventManager::TriggerEvent(unsigned event)
{
	//Right now only one event at the same time can occour
	if(event_triggered)
	{
		return false;
	}

	current_event = event;

	switch (event)
	{
		case 0:
			waves_left = 2;
			enemies_per_wave = 4;
			enemy_manager->SpawnWave(current_event, enemies_per_wave);
			break;
		case 1:
			waves_left = 1;
			enemies_per_wave = 6;
			enemy_manager->SpawnWave(current_event, enemies_per_wave);
			break;
		case 2:
			waves_left = 2;
			enemies_per_wave = 6;
			enemy_manager->SpawnWave(current_event, enemies_per_wave);
			break;

		default:
			return false;
	}

	event_triggered = true;

	return true;
}

