#include "SceneCamerasController.h"

#include "Component/ComponentScript.h"
#include "Component/ComponentTransform.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleScene.h"

#include "UI/Panel/InspectorSubpanel/PanelComponent.h"

#include "imgui.h"



SceneCamerasController* SceneCamerasControllerDLL()
{
	SceneCamerasController* instance = new SceneCamerasController();
	return instance;
}

SceneCamerasController::SceneCamerasController()
{
	panel = new PanelComponent();
}

// Use this for initialization before Start()
void SceneCamerasController::Awake()
{

}

// Use this for initialization
void SceneCamerasController::Start()
{

}

// Update is called once per frame
void SceneCamerasController::Update()
{


}

// Use this for showing variables on inspector
void SceneCamerasController::OnInspector(ImGuiContext* context)
{
	//Necessary to be able to write with imgui
	ImGui::SetCurrentContext(context);

}

//Use this for linking GO automatically
void SceneCamerasController::Save(Config& config) const
{
	config.AddUInt(example->UUID, "ExampleNameforSave");
}

//Use this for linking GO automatically
void SceneCamerasController::Load(const Config& config)
{
	exampleUUID = config.GetUInt("ExampleNameforSave", 0);
}

//Use this for linking GO automatically
void SceneCamerasController::Link()
{
	example = App->scene->GetGameObject(exampleUUID);
}
