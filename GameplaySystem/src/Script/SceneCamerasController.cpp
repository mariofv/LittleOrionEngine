#include "SceneCamerasController.h"

#include "Component/ComponentScript.h"
#include "Component/ComponentTransform.h"
#include "Component/ComponentCamera.h"

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
	camera_list.push_back(main_camera);
	camera_list.push_back(camera_1);
	camera_list.push_back(camera_2);
	camera_list.push_back(camera_3);
	camera_rendering = (ComponentCamera*)main_camera->GetComponent(Component::ComponentType::CAMERA);
}

// Use this for initialization
void SceneCamerasController::Start()
{

}

// Update is called once per frame
void SceneCamerasController::Update()
{
	if (App->input->GetKeyDown(KeyCode::P)) 
	{
		if (index < camera_list.size()-1) 
		{
			++index;
			UpdateCameraRendering();
		}	
	}
	if (App->input->GetKeyDown(KeyCode::O))
	{
		if (index > 0)
		{
			--index;
			UpdateCameraRendering();
		}
	}

}

// Use this for showing variables on inspector
void SceneCamerasController::OnInspector(ImGuiContext* context)
{
	//Necessary to be able to write with imgui
	ImGui::SetCurrentContext(context);
	ShowDraggedObjects();
}

void SceneCamerasController::UpdateCameraRendering()
{
	camera_rendering->Disable();
	camera_rendering = (ComponentCamera*)camera_list[index]->GetComponent(Component::ComponentType::CAMERA);
	camera_rendering->Enable();
}

//Use this for linking GO automatically
//void SceneCamerasController::Save(Config& config) const
//{
//	config.AddUInt(main_camera->UUID, "MainCamera");
//	config.AddUInt(camera_1->UUID, "Camera1");
//	config.AddUInt(camera_2->UUID, "Camera2");
//	config.AddUInt(camera_3->UUID, "Camera3");
//}
//
////Use this for linking GO automatically
//void SceneCamerasController::Load(const Config& config)
//{
//	maincameraUUID = config.GetUInt("MainCamera", 0);
//	camera1UUID = config.GetUInt("Camera1", 0);
//	camera2UUID = config.GetUInt("Camera2", 0);
//	camera3UUID = config.GetUInt("Camera3", 0);
//}
//
////Use this for linking GO automatically
//void SceneCamerasController::Link()
//{
//	main_camera = App->scene->GetGameObject(maincameraUUID);
//	camera_1 = App->scene->GetGameObject(camera1UUID);
//	camera_2 = App->scene->GetGameObject(camera2UUID);
//	camera_3 = App->scene->GetGameObject(camera3UUID);
//}
void SceneCamerasController::InitPublicGameObjects()
{
	//IMPORTANT, public gameobjects, name_gameobjects and go_uuids MUST have same size

	public_gameobjects.push_back(&main_camera);
	public_gameobjects.push_back(&camera_1);
	public_gameobjects.push_back(&camera_2);
	public_gameobjects.push_back(&camera_3);

	variable_names.push_back(GET_VARIABLE_NAME(main_camera));
	variable_names.push_back(GET_VARIABLE_NAME(camera_1));
	variable_names.push_back(GET_VARIABLE_NAME(camera_2));
	variable_names.push_back(GET_VARIABLE_NAME(camera_3));

	for (int i = 0; i < public_gameobjects.size(); ++i)
	{
		name_gameobjects.push_back(is_object);
		go_uuids.push_back(0);
	}
}