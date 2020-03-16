#include "TriggerActionScript.h"

#include "Component/ComponentAABB.h"
#include "Component/ComponentScript.h"
#include "Component/ComponentTransform.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleScene.h"

#include "UI/Panel/InspectorSubpanel/PanelComponent.h"

#include "imgui.h"
#include <MathGeoLib/MathGeoLib.h>

#include "WalkableScript.h"


TriggerActionScript* TriggerActionScriptDLL()
{
	TriggerActionScript *instance = new TriggerActionScript();
	return instance;
}

TriggerActionScript::TriggerActionScript()
{
	panel = new PanelComponent();
}

// Use this for initialization before Start()
void TriggerActionScript::Awake()
{
	std::string aux("WalkableScript");
	movement_component = trigger_go->GetComponentScript(aux);
	movement_script = (WalkableScript*)movement_component->script;
	start_position = trigger_go->transform.GetGlobalTranslation();
}

// Use this for initialization
void TriggerActionScript::Start()
{

}

// Update is called once per frame
void TriggerActionScript::Update()
{
	if(OnTriggerEnter())
	{
		if(movement_script)
		{
			//Do something if inside aabb of an object
			//movement_script->speed *= 5.f;
			trigger_go->transform.SetTranslation(start_position);
		}
	}

}

// Use this for showing variables on inspector
void TriggerActionScript::OnInspector(ImGuiContext* context)
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

bool TriggerActionScript::OnTriggerEnter()
{
	return trigger_go && owner->aabb.global_bounding_box.Intersects(trigger_go->aabb.global_bounding_box);
}
