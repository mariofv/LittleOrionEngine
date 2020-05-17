#include "TriggerGravity.h"

#include "Component/ComponentScript.h"
#include "Component/ComponentTransform.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleScene.h"

#include "EditorUI/Panel/InspectorSubpanel/PanelComponent.h"

#include "imgui.h"

#include "PlayerController.h"

TriggerGravity* TriggerGravityDLL()
{
	TriggerGravity* instance = new TriggerGravity();
	return instance;
}

TriggerGravity::TriggerGravity()
{
	panel = new PanelComponent();
}

// Use this for initialization before Start()
void TriggerGravity::Awake()
{
	player_movement_component = (ComponentScript*)player->GetComponentScript("PlayerController");
	movement_script = (PlayerController*)player_movement_component->script;
}

// Use this for initialization
void TriggerGravity::Start()
{

}

// Update is called once per frame
void TriggerGravity::Update()
{
	if (OnTriggerEnter())
	{
		//movement_script->on_gravity = up_down;
	}
}

bool TriggerGravity::OnTriggerEnter() const
{
	return player && owner->aabb.global_bounding_box.Intersects(player->aabb.global_bounding_box);
}

// Use this for showing variables on inspector
void TriggerGravity::OnInspector(ImGuiContext* context)
{
	//Necessary to be able to write with imgui
	ImGui::SetCurrentContext(context);
	ImGui::Checkbox("Up or down", &up_down);
	ShowDraggedObjects();

}

//Use this for linking JUST GO automatically 
void TriggerGravity::InitPublicGameObjects()
{
	//IMPORTANT, public gameobjects, name_gameobjects and go_uuids MUST have same size

	public_gameobjects.push_back(&player);
	variable_names.push_back(GET_VARIABLE_NAME(player));

	for (unsigned int i = 0; i < public_gameobjects.size(); ++i)
	{
		name_gameobjects.push_back(is_object);
		go_uuids.push_back(0);
	}
}
//Use this for linking GO AND VARIABLES automatically if you need to save variables 
void TriggerGravity::Save(Config& config) const
{
	config.AddBool(up_down, "UpDown");
	Script::Save(config);
}

//Use this for linking GO AND VARIABLES automatically
void TriggerGravity::Load(const Config& config)
{
	config.GetBool("UpDown", up_down);
	Script::Load(config);
}

