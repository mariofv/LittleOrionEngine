#include "EnemyManager.h"

#include "Component/ComponentScript.h"
#include "Component/ComponentTransform.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleScene.h"

#include "EditorUI/Panel/InspectorSubpanel/PanelComponent.h"

#include "imgui.h"

EnemyManager* EnemyManager::instance_singleton = nullptr;

EnemyManager* EnemyManagerDLL()
{
	EnemyManager* instance = new EnemyManager();
	return instance;
}

EnemyManager::EnemyManager()
{
	panel = new PanelComponent();
}

// Use this for initialization before Start()
void EnemyManager::Awake()
{
	instance_singleton = static_cast<EnemyManager*>(owner->GetComponentScript("EnemyManager")->script);
}

// Use this for initialization
void EnemyManager::Start()
{

}

// Update is called once per frame
void EnemyManager::Update()
{

	
}

EnemyManager* EnemyManager::GetInstance()
{
	return instance_singleton;
}

void EnemyManager::AddEnemy(EnemyController * enemy)
{
	enemies.emplace_back(enemy);
}

// Use this for showing variables on inspector
void EnemyManager::OnInspector(ImGuiContext* context)
{
	//Necessary to be able to write with imgui
	ImGui::SetCurrentContext(context);
	ShowDraggedObjects();

}

//Use this for linking JUST GO automatically 
void EnemyManager::InitPublicGameObjects()
{
	for (int i = 0; i < public_gameobjects.size(); ++i)
	{
		name_gameobjects.push_back(is_object);
		go_uuids.push_back(0);
	}
}
//Use this for linking GO AND VARIABLES automatically if you need to save variables 
// void EnemyManager::Save(Config& config) const
// {
// 	config.AddUInt(example->UUID, "ExampleNameforSave");
// 	Script::Save(config);
// }

// //Use this for linking GO AND VARIABLES automatically
// void EnemyManager::Load(const Config& config)
// {
// 	exampleUUID = config.GetUInt("ExampleNameforSave", 0);
// 	Script::Load(config);
// }