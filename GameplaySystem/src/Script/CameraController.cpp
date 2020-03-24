#include "CameraController.h"

#include "Component/ComponentScript.h"
#include "Component/ComponentTransform.h"
#include "Component/ComponentCamera.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleScene.h"
#include "Module/ModuleTime.h"

#include "UI/Panel/InspectorSubpanel/PanelComponent.h"

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

	player_movement_component = player->GetComponentScript("TestScriptRuntime");
	player_movement_script = (TestScriptRuntime*)player_movement_component->script;
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
	ImGui::Text("Game Camera: ");
	ImGui::SameLine();
	ImGui::Button(is_camera.c_str());
	panel->DropGOTarget(camera);
	if (camera) 
	{
		is_camera = camera->name;
	}
	ImGui::Text("Player: ");
	ImGui::SameLine();
	ImGui::Button(is_player.c_str());
	panel->DropGOTarget(player);
	if (player)
	{
		is_player = player->name;
	}
		
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
	float containing_sphere_radius = player->aabb.bounding_box.Size().Length();
	camera_component->is_focusing = true;
	camera_component->SetStartFocusPosition(camera->transform.GetTranslation());
	camera_component->SetGoalFocusPosition(player->aabb.bounding_box.CenterPoint() - camera_frustum.front * 3.f * containing_sphere_radius);
	camera_component->SetFocusTime(App->time->delta_time + 4);
}

//Use this for linking GO automatically
void CameraController::Save(Config& config) const
{
	config.AddUInt(camera->UUID, "Camera");
	config.AddUInt(player->UUID, "Player1");
}

//Use this for linking GO automatically
void CameraController::Load(const Config& config)
{
	cameraUUID = config.GetUInt("Camera", 0);
	playerUUID = config.GetUInt("Player1", 0);
}

//Use this for linking GO automatically
void CameraController::Link()
{
	camera = App->scene->GetGameObject(cameraUUID);
	player = App->scene->GetGameObject(playerUUID);
}
