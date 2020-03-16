#include "TemplateScript.h"

#include "Component/ComponentScript.h"
#include "Component/ComponentTransform.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleScene.h"

#include "UI/Panel/InspectorSubpanel/PanelComponent.h"

#include "imgui.h"



TemplateScript* TemplateScriptDLL()
{
	TemplateScript *instance = new TemplateScript();
	return instance;
}

TemplateScript::TemplateScript()
{
	panel = new PanelComponent();
}

// Use this for initialization before Start()
void TemplateScript::Awake()
{

}

// Use this for initialization
void TemplateScript::Start()
{

}

// Update is called once per frame
void TemplateScript::Update()
{


}

// Use this for showing variables on inspector
void TemplateScript::OnInspector(ImGuiContext* context)
{
	//Necessary to be able to write with imgui
	ImGui::SetCurrentContext(context);

}