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

	player_movement_component = player1->GetComponentScript("PlayerController");
	player_movement_script = (PlayerController*)player_movement_component->script;
	rotation = owner->transform.GetRotation();

	selected_offset = offset_near;

}

// Use this for initialization
void CameraController::Start()
{
	//camera_component->SetOrthographicView();
	if (!multiplayer)
	{
		float3 current_position = player1->transform.GetTranslation();
		float3 position_to_focus = float3(current_position.x, current_position.y + selected_offset.y, /*(current_position.z / 2) +*/ selected_offset.z);
		Focus(position_to_focus);
	}
	else
	{
		float x_distance = abs(player1->transform.GetTranslation().x - player2->transform.GetTranslation().x);
		float y_distance = abs(player1->transform.GetTranslation().y - player2->transform.GetTranslation().y);
		float z_distance = abs(player1->transform.GetTranslation().z - player2->transform.GetTranslation().z);

		float min_x = math::Min(player1->transform.GetTranslation().x, player2->transform.GetTranslation().x);
		float min_y = math::Min(player1->transform.GetTranslation().y, player2->transform.GetTranslation().y);
		float min_z = math::Min(player1->transform.GetTranslation().z, player2->transform.GetTranslation().z);
		Focus(float3(min_x + (x_distance / 2), min_y + (y_distance / 2) + selected_offset.y, min_z + (z_distance / 2) + selected_offset.z));
	}
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
		if(multiplayer)
		{
			MultiplayerCamera();
		}
		else
		{
			FollowPlayer();
		}
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
	ImGui::Checkbox("Multiplayer", &multiplayer);
	ImGui::Checkbox("Is Focusing", &is_focusing);
	ImGui::DragFloat3("Offset", selected_offset.ptr(), 0.5f, 0.f, 100.f);
	ImGui::DragFloat("Distance", &distance, 0.5f, 0.f, 100.f);
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

void CameraController::Focus(float3 position_to_focus)
{

	float focus_progress = math::Min((App->time->delta_time - start_focus_time) / 100.f, 1.f);
	float3 new_camera_position = owner->transform.GetTranslation().Lerp(position_to_focus, focus_progress);
	owner->transform.SetTranslation(float3(new_camera_position.x, position_to_focus.y, new_camera_position.z));
	is_focusing = focus_progress != 1;

}

void CameraController::FollowPlayer() 
{
	distance = abs(player1->transform.GetTranslation().x - owner->transform.GetTranslation().x);
	if ( distance > 2.f && !is_focusing)
	{
		start_focus_time = App->time->delta_time;
		is_focusing = true;
	}

	if (is_focusing)
	{
		float3 current_position = player1->transform.GetTranslation();
		float3 position_to_focus = float3(current_position.x, current_position.y + selected_offset.y, (current_position.z / 2) + selected_offset.z);
		Focus(position_to_focus);
	}

}
void CameraController::MultiplayerCamera()
{
	float x_distance = abs(player1->transform.GetTranslation().x - player2->transform.GetTranslation().x);
	float y_distance = abs(player1->transform.GetTranslation().y - player2->transform.GetTranslation().y);
	float z_distance = abs(player1->transform.GetTranslation().z - player2->transform.GetTranslation().z);

	float min_x = math::Min(player1->transform.GetTranslation().x, player2->transform.GetTranslation().x);
	float min_y = math::Min(player1->transform.GetTranslation().y, player2->transform.GetTranslation().y);
	float min_z = math::Min(player1->transform.GetTranslation().z, player2->transform.GetTranslation().z);

	if (x_distance > 5 )
	{
		selected_offset = offset_far;
		if (!is_focusing)
		{
			start_focus_time = App->time->delta_time;
			is_focusing = true;
		}

	}

	if (x_distance < 5)
	{
		selected_offset = offset_near;
		if(!is_focusing)
		{
			start_focus_time = App->time->delta_time;
			is_focusing = true;
		}
	}

	if(is_focusing)
	{
		Focus(float3(min_x + (x_distance/2), min_y + (y_distance/2) + selected_offset.y, min_z + (z_distance / 2) + selected_offset.z));
	}

}

void CameraController::InitPublicGameObjects()
{
	//IMPORTANT, public gameobjects, name_gameobjects and go_uuids MUST have same size

	public_gameobjects.push_back(&camera);
	public_gameobjects.push_back(&player1);
	public_gameobjects.push_back(&player2);
	public_gameobjects.push_back(&debug);

	variable_names.push_back(GET_VARIABLE_NAME(camera));
	variable_names.push_back(GET_VARIABLE_NAME(player1));
	variable_names.push_back(GET_VARIABLE_NAME(player2));
	variable_names.push_back(GET_VARIABLE_NAME(debug));

	for (unsigned int i = 0; i < public_gameobjects.size(); ++i)
	{
		name_gameobjects.push_back(is_object);
		go_uuids.push_back(0);
	}
}