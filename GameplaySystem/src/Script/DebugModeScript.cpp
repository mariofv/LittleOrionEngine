#include "DebugModeScript.h"

#include "Component/ComponentImage.h"
#include "Component/ComponentScript.h"
#include "Component/ComponentTransform.h"
#include "Component/ComponentText.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleRender.h"
#include "Module/ModuleScene.h"
#include "Module/ModuleTime.h"

#include "EditorUI/Panel/InspectorSubpanel/PanelComponent.h"

#include "imgui.h"



DebugModeScript* DebugModeScriptDLL()
{
	DebugModeScript* instance = new DebugModeScript();
	return instance;
}

DebugModeScript::DebugModeScript()
{
	panel = new PanelComponent();
}

// Use this for initialization before Start()
void DebugModeScript::Awake()
{
	background = (ComponentImage*)background_go->GetComponentUI(ComponentUI::UIType::IMAGE);
	text_fps = (ComponentText*)text_fps_go->GetComponentUI(ComponentUI::UIType::TEXT);
	text_tris = (ComponentText*)text_tris_go->GetComponentUI(ComponentUI::UIType::TEXT);
	text_verts = (ComponentText*)text_verts_go->GetComponentUI(ComponentUI::UIType::TEXT);
}

// Use this for initialization
void DebugModeScript::Start()
{

}

// Update is called once per frame
void DebugModeScript::Update()
{
	if (debug_enabled)
	{
		text_fps->text = base_str_fps + std::to_string(App->time->GetFPS()) + std::to_string('(') + std::to_string(App->time->delta_time) + std::to_string('ms)');
		text_tris->text = base_str_tris + std::to_string(App->renderer->GetRenderedTris());
		text_verts->text = base_str_verts + std::to_string(App->renderer->GetRenderedVerts());
	}

}

// Use this for showing variables on inspector
void DebugModeScript::OnInspector(ImGuiContext* context)
{
	//Necessary to be able to write with imgui
	ImGui::SetCurrentContext(context);
	ShowDraggedObjects();

}

//Use this for linking JUST GO automatically 
void DebugModeScript::InitPublicGameObjects()
{
	//IMPORTANT, public gameobjects, name_gameobjects and go_uuids MUST have same size

	public_gameobjects.push_back(&background_go);
	variable_names.push_back(GET_VARIABLE_NAME(background_go));

	public_gameobjects.push_back(&text_tris_go);
	variable_names.push_back(GET_VARIABLE_NAME(text_tris_go));

	public_gameobjects.push_back(&text_verts_go);
	variable_names.push_back(GET_VARIABLE_NAME(text_verts_go));

	public_gameobjects.push_back(&text_fps_go);
	variable_names.push_back(GET_VARIABLE_NAME(text_fps_go));

	for (int i = 0; i < public_gameobjects.size(); ++i)
	{
		name_gameobjects.push_back(is_object);
		go_uuids.push_back(0);
	}
}
//Use this for linking GO AND VARIABLES automatically if you need to save variables 
// void DebugModeScript::Save(Config& config) const
// {
// 	config.AddUInt(example->UUID, "ExampleNameforSave");
// 	Script::Save(config);
// }

// //Use this for linking GO AND VARIABLES automatically
// void DebugModeScript::Load(const Config& config)
// {
// 	exampleUUID = config.GetUInt("ExampleNameforSave", 0);
// 	Script::Load(config);
// }

