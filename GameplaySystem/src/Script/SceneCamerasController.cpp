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
		if (index < camera_list.size()) 
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
	ImGui::Text("Game Camera: ");
	ImGui::SameLine();
	ImGui::Button(is_main_camera.c_str());
	panel->DropGOTarget(main_camera);
	if (main_camera)
	{
		is_main_camera = main_camera->name;
	}
	ImGui::Text("Camera 1: ");
	ImGui::SameLine();
	ImGui::Button(is_camera_1.c_str());
	panel->DropGOTarget(camera_1);
	if (camera_1)
	{
		is_camera_1 = camera_1->name;
	}
	ImGui::Text("Camera 2: ");
	ImGui::SameLine();
	ImGui::Button(is_camera_2.c_str());
	panel->DropGOTarget(camera_2);
	if (camera_2)
	{
		is_camera_2 = camera_2->name;
	}
	ImGui::Text("Camera 3: ");
	ImGui::SameLine();
	ImGui::Button(is_camera_3.c_str());
	panel->DropGOTarget(camera_3);
	if (camera_3)
	{
		is_camera_3 = camera_3->name;
	}

}

void SceneCamerasController::UpdateCameraRendering()
{
	camera_rendering->Disable();
	camera_rendering = (ComponentCamera*)camera_list[index]->GetComponent(Component::ComponentType::CAMERA);
	camera_rendering->Enable();
}

//Use this for linking GO automatically
void SceneCamerasController::Save(Config& config) const
{
	config.AddUInt(main_camera->UUID, "MainCamera");
	config.AddUInt(camera_1->UUID, "Camera1");
	config.AddUInt(camera_2->UUID, "Camera2");
	config.AddUInt(camera_3->UUID, "Camera3");
}

//Use this for linking GO automatically
void SceneCamerasController::Load(const Config& config)
{
	maincameraUUID = config.GetUInt("MainCamera", 0);
	camera1UUID = config.GetUInt("Camera1", 0);
	camera2UUID = config.GetUInt("Camera2", 0);
	camera2UUID = config.GetUInt("Camera3", 0);
}

//Use this for linking GO automatically
void SceneCamerasController::Link()
{
	example = App->scene->GetGameObject(exampleUUID);
}
