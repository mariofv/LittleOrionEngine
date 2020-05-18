#include "DamageIndicatorSpawner.h"

#include "Component/ComponentCamera.h"
#include "Component/ComponentCanvas.h"
#include "Component/ComponentScript.h"
#include "Component/ComponentTransform.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleCamera.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleScene.h"
#include "Module/ModuleUI.h"

#include "EditorUI/Panel/InspectorSubpanel/PanelComponent.h"

#include "imgui.h"


DamageIndicatorSpawner* DamageIndicatorSpawnerDLL()
{
	DamageIndicatorSpawner* instance = new DamageIndicatorSpawner();
	return instance;
}

DamageIndicatorSpawner::DamageIndicatorSpawner()
{
	panel = new PanelComponent();
}

// Use this for initialization before Start()
void DamageIndicatorSpawner::Awake()
{

}

// Use this for initialization
void DamageIndicatorSpawner::Start()
{
	damage_indicators_game_objects[0] = damage_indicator_game_object_0;
	damage_indicators_game_objects[1] = damage_indicator_game_object_1;
	damage_indicators_game_objects[2] = damage_indicator_game_object_2;
	damage_indicators_game_objects[3] = damage_indicator_game_object_3;
	damage_indicators_game_objects[4] = damage_indicator_game_object_4;
}

// Update is called once per frame
void DamageIndicatorSpawner::Update()
{


}

void DamageIndicatorSpawner::SpawnDamageIndicator(int damage, float3 position)
{
	float4 position_float4 = float4(position, 1.f);
	float4 clip_coordinates = App->cameras->main_camera->GetClipMatrix() * position_float4;
	float3 device_coordinates = clip_coordinates.xyz() / clip_coordinates.w;

	float2 canvas_position = float2
	(
		device_coordinates.x * App->ui->main_canvas->GetCanvasScreenSize().x / 2.f,
		device_coordinates.y * App->ui->main_canvas->GetCanvasScreenSize().y / 2.f
	);

	GameObject* damage_indicator_game_object = GetAvailableDamageIndicator();

	damage_indicator_game_object->transform_2d.SetTranslation(float3(canvas_position, 0.f));
}

GameObject* DamageIndicatorSpawner::GetAvailableDamageIndicator()
{
	return damage_indicators_game_objects[0];
}

// Use this for showing variables on inspector
void DamageIndicatorSpawner::OnInspector(ImGuiContext* context)
{
	//Necessary to be able to write with imgui
	ImGui::SetCurrentContext(context);
	ShowDraggedObjects();
}

//Use this for linking JUST GO automatically 
void DamageIndicatorSpawner::InitPublicGameObjects()
{
	//IMPORTANT, public gameobjects, name_gameobjects and go_uuids MUST have same size

	public_gameobjects.push_back(&damage_indicator_game_object_0);
	variable_names.push_back(GET_VARIABLE_NAME(damage_indicator_game_object_0));

	public_gameobjects.push_back(&damage_indicator_game_object_1);
	variable_names.push_back(GET_VARIABLE_NAME(damage_indicator_game_object_1));

	public_gameobjects.push_back(&damage_indicator_game_object_2);
	variable_names.push_back(GET_VARIABLE_NAME(damage_indicator_game_object_2));

	public_gameobjects.push_back(&damage_indicator_game_object_3);
	variable_names.push_back(GET_VARIABLE_NAME(damage_indicator_game_object_3));

	public_gameobjects.push_back(&damage_indicator_game_object_4);
	variable_names.push_back(GET_VARIABLE_NAME(damage_indicator_game_object_4));


	for (int i = 0; i < public_gameobjects.size(); ++i)
	{
		name_gameobjects.push_back(is_object);
		go_uuids.push_back(0);
	}
}

//Use this for linking GO AND VARIABLES automatically if you need to save variables 
// void DamageIndicatorSpawner::Save(Config& config) const
// {
// 	config.AddUInt(example->UUID, "ExampleNameforSave");
// 	Script::Save(config);
// }

// //Use this for linking GO AND VARIABLES automatically
// void DamageIndicatorSpawner::Load(const Config& config)
// {
// 	exampleUUID = config.GetUInt("ExampleNameforSave", 0);
// 	Script::Load(config);
// }