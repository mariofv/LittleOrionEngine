#include "PlayerController.h"

#include "Component/ComponentScript.h"
#include "Component/ComponentTransform.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleScene.h"

#include "UI/Panel/InspectorSubpanel/PanelComponent.h"

#include "imgui.h"



PlayerController* PlayerControllerDLL()
{
	PlayerController* instance = new PlayerController();
	return instance;
}

PlayerController::PlayerController()
{
	panel = new PanelComponent();
}

// Use this for initialization before Start()
void PlayerController::Awake()
{

}

// Use this for initialization
void PlayerController::Start()
{

}

// Update is called once per frame
void PlayerController::Update()
{


}

// Use this for showing variables on inspector
void PlayerController::OnInspector(ImGuiContext* context)
{
	//Necessary to be able to write with imgui
	ImGui::SetCurrentContext(context);

}

//Use this for linking GO automatically
void PlayerController::Save(Config& config) const
{
	config.AddUInt(example->UUID, "ExampleNameforSave");
}

//Use this for linking GO automatically
void PlayerController::Load(const Config& config)
{
	exampleUUID = config.GetUInt("ExampleNameforSave", 0);
}

//Use this for linking GO automatically
void PlayerController::Link()
{
	example = App->scene->GetGameObject(exampleUUID);
}
