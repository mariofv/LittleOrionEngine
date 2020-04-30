#include "ExampleScript.h"

#include "Component/ComponentScript.h"
#include "Component/ComponentTransform.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleScene.h"

#include "EditorUI/Panel/InspectorSubpanel/PanelComponent.h"

#include "imgui.h"



ExampleScript* ExampleScriptDLL()
{
	ExampleScript *instance = new ExampleScript();
	return instance;
}

ExampleScript::ExampleScript()
{
	panel = new PanelComponent();
}

void ExampleScript::Awake()
{
	//Example Unity-like of how get components from GO dragged
	//script_component = enemy->GetComponentScript("Script_i_want");
	//example_script = (Script_i_want*)enemy_component->script;
	
}

void ExampleScript::Start()
{
}

void ExampleScript::Update()
{

	//Test();

	//if (App->input->GetKeyDown(KeyCode::C))
	//{
	//	GameObject* go = App->scene->CreateGameObject();
	//}
}

void ExampleScript::OnInspector(ImGuiContext* context)
{
	//Necessary to be able to write with imgui
	ImGui::SetCurrentContext(context);
	//Example to show text
	ImGui::Text("Example Script Inspector");
	//Example Showing variables and being able to modify it on Runtime.
	ImGui::DragFloat("Speed", &speed, 0.01f, 0.f, 0.5f);
	ImGui::DragFloat("Rotation Speed", &rotation_speed, 0.01f, 0.f, 0.5f);
	//Example to Drag and drop and link GOs in the Editor, Unity-like (WIP)
	ShowDraggedObjects();
}

void ExampleScript::Test()
{
	//example how to get variables from the engine
	float3 transform = owner->transform.GetTranslation();
	float3 rotation = owner->transform.GetRotationRadiants();
	//EXAMPLE USING PLAYER INPUT
	if (App->input->GetKey(KeyCode::A))
	{
		owner->transform.SetTranslation(float3(transform.x + speed, transform.y, transform.z));
	}
	if (App->input->GetKey(KeyCode::W))
	{
		owner->transform.SetTranslation(float3(transform.x, transform.y, transform.z + speed));
	}
	if (App->input->GetKey(KeyCode::S))
	{
		owner->transform.SetTranslation(float3(transform.x, transform.y, transform.z - speed));
	}
	if (App->input->GetKey(KeyCode::D))
	{
		owner->transform.SetTranslation(float3(transform.x - speed, transform.y, transform.z));
	}

}
//Example how to Save the GO that we want to stay linked after loading/saving and then Linking 
void ExampleScript::InitPublicGameObjects()
{
	//IMPORTANT, public gameobjects, name_gameobjects and go_uuids MUST have same size

	public_gameobjects.push_back(&example);

	variable_names.push_back(GET_VARIABLE_NAME(example));

	for (unsigned int i = 0; i < public_gameobjects.size(); ++i)
	{
		name_gameobjects.push_back(is_object);
		go_uuids.push_back(0);
	}
}
//Example how to Save the GO and SOME VARIABLES that we want to stay linked after loading/saving adn then Linking
//IN CASE YOU DON'T NEED TO STORE ANYTHING ELSE THAN GO YOU DON'T NEED THIS TWO METHODS
void ExampleScript::Save(Config& config) const
{
	//config.AddUInt(speed, "speed");
	//Script::Save(config);
}

void ExampleScript::Load(const Config& config)
{
	//speed = config.GetUInt("speed", 0);
	//Script::Load(config);
}

