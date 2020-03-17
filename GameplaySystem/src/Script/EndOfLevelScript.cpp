#include "EndOfLevelScript.h"

#include "Component/ComponentScript.h"
#include "Component/ComponentTransform.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleEditor.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleScene.h"

#include "UI/Panel/InspectorSubpanel/PanelComponent.h"

#include "imgui.h"



EndOfLevelScript* EndOfLevelScriptDLL()
{
	EndOfLevelScript *instance = new EndOfLevelScript();
	return instance;
}

EndOfLevelScript::EndOfLevelScript()
{
	panel = new PanelComponent();
}

// Use this for initialization before Start()
void EndOfLevelScript::Awake()
{

}

// Use this for initialization
void EndOfLevelScript::Start()
{

}

// Update is called once per frame
void EndOfLevelScript::Update()
{
	if (OnTriggerEnter())
	{
		App->editor->OpenScene(level_to_load_path);
	}

}

// Use this for showing variables on inspector
void EndOfLevelScript::OnInspector(ImGuiContext* context)
{
	//Necessary to be able to write with imgui
	ImGui::SetCurrentContext(context);
	//Example to show text
	ImGui::Text("TriggerAction Script Inspector");
	//Example to Drag and drop and link GOs in the Editor, Unity-like (WIP)
	ImGui::Text("TestScriptRuntime: ");
	ImGui::SameLine();
	ImGui::Button(is_object.c_str());
	panel->DropGOTarget(trigger_go, "TestScriptRuntime");
	if (trigger_go)
		is_object = trigger_go->name;
}

bool EndOfLevelScript::OnTriggerEnter()
{
	bool is_inside = trigger_go && owner->aabb.global_bounding_box.Intersects(trigger_go->aabb.global_bounding_box) && !entered;
	if (is_inside)
	{
		entered = true;
	}
	return is_inside ;
}
