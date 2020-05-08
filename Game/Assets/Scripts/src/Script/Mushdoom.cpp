#include "Mushdoom.h"

#include "Component/ComponentScript.h"
#include "Component/ComponentTransform.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleScene.h"

#include "EditorUI/Panel/InspectorSubpanel/PanelComponent.h"

#include "imgui.h"

Mushdoom* MushdoomDLL()
{
	Mushdoom* instance = new Mushdoom();
	return instance;
}

Mushdoom::Mushdoom()
{
	type = EnemyType::MUSHDOOM;
	panel = new PanelComponent();
}

// Use this for initialization before Start()
void Mushdoom::Awake()
{
	EnemyController::Awake();
}

// Use this for initialization
void Mushdoom::Start()
{
	EnemyController::Start();
}

// Update is called once per frame
void Mushdoom::Update()
{
	EnemyController::Update();
}

void Mushdoom::ResetEnemy()
{
	health_points = max_health_points;

	is_alive = true;
	is_attacking = false;

	owner->transform.SetTranslation(init_translation);
	owner->transform.SetRotation(init_rotation);
	owner->transform.SetScale(init_scale);
}

// Use this for showing variables on inspector
//void Mushdoom::OnInspector(ImGuiContext* context)
//{
//	//Necessary to be able to write with imgui
//	ImGui::SetCurrentContext(context);
//	ShowDraggedObjects();
//
//}

//Use this for linking JUST GO automatically 
//void Mushdoom::InitPublicGameObjects()
//{
//	//IMPORTANT, public gameobjects, name_gameobjects and go_uuids MUST have same size
//
//	public_gameobjects.push_back(&example);
//	variable_names.push_back(GET_VARIABLE_NAME(example));
//
//	for (int i = 0; i < public_gameobjects.size(); ++i)
//	{
//		name_gameobjects.push_back(is_object);
//		go_uuids.push_back(0);
//	}
//}

//Use this for linking GO AND VARIABLES automatically if you need to save variables 
// void Mushdoom::Save(Config& config) const
// {
// 	config.AddUInt(example->UUID, "ExampleNameforSave");
// 	Script::Save(config);
// }

// //Use this for linking GO AND VARIABLES automatically
// void Mushdoom::Load(const Config& config)
// {
// 	exampleUUID = config.GetUInt("ExampleNameforSave", 0);
// 	Script::Load(config);
// }