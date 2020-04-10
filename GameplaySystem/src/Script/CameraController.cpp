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
	rotation = owner->transform.GetRotation();
}

// Use this for initialization
void CameraController::Start()
{
	//camera_component->SetOrthographicView();
}

// Update is called once per frame
void CameraController::Update()
{

	if (App->input->GetKey(KeyCode::Alpha1))
	{
		owner->transform.SetRotation(rotation);
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
	if (App->input->GetKey(KeyCode::UpArrow))
	{
		camera_component->RotatePitch(-rotation_speed);
	}
	if (App->input->GetKey(KeyCode::LeftArrow))
	{
		camera_component->RotateYaw(-rotation_speed);
	}
	if (App->input->GetKey(KeyCode::DownArrow))
	{
		camera_component->RotatePitch(rotation_speed);
	}
	if (App->input->GetKey(KeyCode::RightArrow))
	{
		camera_component->RotateYaw(rotation_speed);
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
	float3 offset(0.f, 15.f, 24.f);
	float3 new_position = player->transform.GetTranslation() + offset;
	owner->transform.SetTranslation(new_position);

}

void CameraController::InitPublicGameObjects()
{
	//IMPORTANT, public gameobjects, name_gameobjects and go_uuids MUST have same size

	public_gameobjects.push_back(&camera);
	public_gameobjects.push_back(&player);

	variable_names.push_back(GET_VARIABLE_NAME(camera));
	variable_names.push_back(GET_VARIABLE_NAME(player));

	for (unsigned int i = 0; i < public_gameobjects.size(); ++i)
	{
		name_gameobjects.push_back(is_object);
		go_uuids.push_back(0);
	}
}