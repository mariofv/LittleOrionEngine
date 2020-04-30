#include "DamageScript.h"

#include "Component/ComponentScript.h"
#include "Component/ComponentTransform.h"
#include "Component/ComponentProgressBar.h"
#include "Component/ComponentUI.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleScene.h"
#include "Module/ModuleTime.h"

#include "EditorUI/Panel/InspectorSubpanel/PanelComponent.h"

#include "imgui.h"



DamageScript* DamageScriptDLL()
{
	DamageScript* instance = new DamageScript();
	return instance;
}

DamageScript::DamageScript()
{
	panel = new PanelComponent();
}

// Use this for initialization before Start()
void DamageScript::Awake()
{
	health_component = (ComponentProgressBar*)health_bar->GetComponent(ComponentUI::UIType::PROGRESSBAR);
}

// Use this for initialization
void DamageScript::Start()
{

}

// Update is called once per frame
void DamageScript::Update()
{
	if (OnTriggerEnter())
	{
		float aux_health = health_component->percentage;
		aux_health -= damage * App->time->delta_time / 1000.f;
		health_component->percentage = math::Clamp(aux_health, 0.0f, 100.0f);
	}
}

bool DamageScript::OnTriggerEnter() const
{
	return player && owner->aabb.global_bounding_box.Intersects(player->aabb.global_bounding_box);
}

// Use this for showing variables on inspector
void DamageScript::OnInspector(ImGuiContext* context)
{
	//Necessary to be able to write with imgui
	ImGui::SetCurrentContext(context);
	ShowDraggedObjects();

}

//Use this for linking JUST GO automatically 
void DamageScript::InitPublicGameObjects()
{
	//IMPORTANT, public gameobjects, name_gameobjects and go_uuids MUST have same size

	public_gameobjects.push_back(&player);
	variable_names.push_back(GET_VARIABLE_NAME(player));
	public_gameobjects.push_back(&health_bar);
	variable_names.push_back(GET_VARIABLE_NAME(health_bar));

	for (unsigned int i = 0; i < public_gameobjects.size(); ++i)
	{
		name_gameobjects.push_back(is_object);
		go_uuids.push_back(0);
	}
}
//Use this for linking GO AND VARIABLES automatically if you need to save variables 
// void DamageScript::Save(Config& config) const
// {
// 	config.AddUInt(example->UUID, "ExampleNameforSave");
// 	Script::Save(config);
// }

// //Use this for linking GO AND VARIABLES automatically
// void DamageScript::Load(const Config& config)
// {
// 	exampleUUID = config.GetUInt("ExampleNameforSave", 0);
// 	Script::Load(config);
// }

