#include "CameraController.h"

#include "Component/ComponentScript.h"
#include "Component/ComponentTransform.h"
#include "Component/ComponentCamera.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleScene.h"
#include "Module/ModuleTime.h"

#include "EditorUI/Panel/InspectorSubpanel/PanelComponent.h"

#include "imgui.h"




CameraController* CameraControllerDLL()
{
	CameraController* instance = new CameraController();
	return instance;
}

CameraController::CameraController()
{
	panel = new PanelComponent();
}

// Use this for initialization before Start()
void CameraController::Awake()
{

	camera_component = (ComponentCamera*)camera->GetComponent(Component::ComponentType::CAMERA);

	player_movement_component = player->GetComponentScript("PlayerController");
	player_movement_script = (PlayerController*)player_movement_component->script;
	containing_sphere_radius = player->aabb.global_bounding_box.Size().Length();
}

// Use this for initialization
void CameraController::Start()
{
	//camera_component->SetOrthographicView();
}

// Update is called once per frame
void CameraController::Update()
{

	if (App->input->GetKey(KeyCode::J))
	{
		god_mode = !god_mode;
		ActivePlayer();
	}
	if (god_mode) 
	{
		GodCamera();
	}
	else 
	{
		FollowPlayer();
	}

}

// Use this for showing variables on inspector
void CameraController::OnInspector(ImGuiContext* context)
{
	//Necessary to be able to write with imgui
	ImGui::SetCurrentContext(context);
	ImGui::Text("Camera Controller Inspector");
	//Example to Drag and drop and link GOs in the Editor, Unity-like (WIP)
	ImGui::Text("Variables: ");
	ShowDraggedObjects();
}

void CameraController::GodCamera() 
{
	if (App->input->GetKey(KeyCode::A))
	{
		camera_component->MoveLeft();
	}
	if (App->input->GetKey(KeyCode::W))
	{
		camera_component->MoveForward();
	}
	if (App->input->GetKey(KeyCode::S))
	{
		camera_component->MoveBackward();
	}
	if (App->input->GetKey(KeyCode::D))
	{
		camera_component->MoveRight();
	}
	if (App->input->GetKey(KeyCode::E))
	{
		camera_component->MoveDown();
	}
	if (App->input->GetKey(KeyCode::Q))
	{
		camera_component->MoveUp();
	}
	//TODO MOVE AND ROTATE WITH JOYSTICK
}

void CameraController::ActivePlayer()
{
	if (god_mode) 
	{
		player_movement_component->Disable();
	}
	else 
	{
		player_movement_component->Enable();
	}

}

void CameraController::FollowPlayer() 
{
	CenterToPlayer();
}

void CameraController::CenterToPlayer()
{
	Frustum camera_frustum = camera_component->GetFrustum();
	camera_component->is_focusing = true;
	camera_component->SetStartFocusPosition(camera->transform.GetTranslation());
	camera_component->SetGoalFocusPosition(player->aabb.global_bounding_box.CenterPoint() - camera_frustum.front * 3.f * containing_sphere_radius);
	camera_component->SetFocusTime(App->time->delta_time + 2000.f);
}

//Use this for linking GO automatically
//void CameraController::Save(Config& config) const
//{
//	config.AddUInt(camera->UUID, "Camera");
//	config.AddUInt(player->UUID, "Player1");
//}
//
////Use this for linking GO automatically
//void CameraController::Load(const Config& config)
//{
//	cameraUUID = config.GetUInt("Camera", 0);
//	playerUUID = config.GetUInt("Player1", 0);
//}
//
////Use this for linking GO automatically
//void CameraController::Link()
//{
//	camera = App->scene->GetGameObject(cameraUUID);
//	player = App->scene->GetGameObject(playerUUID);
//}

void CameraController::InitPublicGameObjects()
{
	//IMPORTANT, public gameobjects, name_gameobjects and go_uuids MUST have same size

	public_gameobjects.push_back(&camera);
	public_gameobjects.push_back(&player);

	variable_names.push_back(GET_VARIABLE_NAME(camera));
	variable_names.push_back(GET_VARIABLE_NAME(player));

	for (int i = 0; i < public_gameobjects.size(); ++i)
	{
		name_gameobjects.push_back(is_object);
		go_uuids.push_back(0);
	}
}