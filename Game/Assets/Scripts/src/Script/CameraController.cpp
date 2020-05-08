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

#include "DebugModeScript.h"

#include "imgui.h"
#include <math.h>

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

	ComponentScript* component_debug = debug->GetComponentScript("DebugModeScript");
	debug_mode = (DebugModeScript*)component_debug->script;

	player_movement_component = player->GetComponentScript("PlayerController");
	player_movement_script = (PlayerController*)player_movement_component->script;
	rotation = owner->transform.GetRotation();
	if(App->input->singleplayer_input)
	{
		offset = float3(0.f, 5.5f, 11.f);
	}	
}

// Use this for initialization
void CameraController::Start()
{
	//camera_component->SetOrthographicView();
}

// Update is called once per frame
void CameraController::Update()
{
	if (!debug_mode->debug_enabled && god_mode) 
	{
		owner->transform.SetRotation(rotation);
		god_mode = !god_mode;
		ActivePlayer();
	}
	if (App->input->GetKeyDown(KeyCode::Alpha1)||App->input->GetControllerButtonDown(ControllerCode::Back, ControllerID::ONE))
	{ 
		if(debug_mode->debug_enabled || god_mode)
		{
			owner->transform.SetRotation(rotation);
			god_mode = !god_mode;
			ActivePlayer();
		}
	}
	if (god_mode && debug_mode->debug_enabled)
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
	if (App->input->GetKey(KeyCode::E) || App->input->GetControllerButton(ControllerCode::DownDpad, ControllerID::ONE))
	{
		camera_component->MoveDown();
	}
	if (App->input->GetKey(KeyCode::Q) || App->input->GetControllerButton(ControllerCode::UpDpad, ControllerID::ONE))
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
	float2 right_axis = App->input->GetAxisControllerRaw(ControllerAxis::RIGHT_JOYSTICK_RAW, ControllerID::ONE);
	float3 right_axis_direction = float3(right_axis.x, 0.0f, right_axis.y);

	if (!right_axis_direction.Equals(float3::zero))
	{
		float3 direction = right_axis_direction * rotation_speed +  owner->transform.GetTranslation();;
		owner->transform.LookAt(direction);
	}
	float2 left_axis = App->input->GetAxisControllerRaw(ControllerAxis::LEFT_JOYSTICK_RAW, ControllerID::ONE);
	float3 left_axis_direction = float3(left_axis.x, 0.0f, left_axis.y);

	if (!left_axis_direction.Equals(float3::zero))
	{
		float3 direction = left_axis_direction * rotation_speed + owner->transform.GetTranslation();;
		owner->transform.SetTranslation(direction);
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
void CameraController::Focus()
{
	if (is_focusing)
	{
		float focus_progress = math::Min((App->time->delta_time - start_focus_time) / 100.f, 1.f);
		float3 new_camera_position = owner->transform.GetTranslation().Lerp(player->transform.GetTranslation(), focus_progress);
		owner->transform.SetTranslation(float3(new_camera_position.x + offset.x, player->transform.GetTranslation().y + offset.y, offset.z));
		is_focusing = focus_progress != 1;
	}

}
void CameraController::FollowPlayer() 
{
	float distance = abs(player->transform.GetTranslation().x - owner->transform.GetTranslation().x);
	if ( distance > 2 && !is_focusing)
	{
		start_focus_time = App->time->delta_time;
		is_focusing = true;
	}
	if(is_focusing)
	{
		Focus();
	}
	//float3 new_position = player->transform.GetTranslation() + offset;
	//owner->transform.SetTranslation(new_position);

}
void CameraController::MultiplayerCamera()
{
	float distance = abs(player->transform.GetTranslation().x - owner->transform.GetTranslation().x);
	if (distance > 2 && !is_focusing)
	{
		start_focus_time = App->time->delta_time;
		is_focusing = true;
	}
	if (is_focusing)
	{
		Focus();
	}

}

void CameraController::InitPublicGameObjects()
{
	//IMPORTANT, public gameobjects, name_gameobjects and go_uuids MUST have same size

	public_gameobjects.push_back(&camera);
	public_gameobjects.push_back(&player);
	public_gameobjects.push_back(&debug);

	variable_names.push_back(GET_VARIABLE_NAME(camera));
	variable_names.push_back(GET_VARIABLE_NAME(player));
	variable_names.push_back(GET_VARIABLE_NAME(debug));

	for (unsigned int i = 0; i < public_gameobjects.size(); ++i)
	{
		name_gameobjects.push_back(is_object);
		go_uuids.push_back(0);
	}
}