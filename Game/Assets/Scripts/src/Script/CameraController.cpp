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

	player_movement_component = player1->GetComponentScript("PlayerController");
	player_movement_script = (PlayerController*)player_movement_component->script;

	player2_movement_component = player2->GetComponentScript("PlayerController");
	player2_movement_script = (PlayerController*)player2_movement_component->script;

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
	if(!freeze)
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
	ImGui::DragFloat("Distance", &distance_x, 0.5f, 0.f, 100.f);
}

void CameraController::ActivePlayer()
{
	if (god_mode) 
	{
		player_movement_component->Disable();
		if(App->input->singleplayer_input)
		{
			player2_movement_component->Disable();
		}
	}
	else 
	{
		player_movement_component->Enable();
		if (App->input->singleplayer_input)
		{
			player2_movement_component->Enable();
		}
	}

}

void CameraController::Focus(float3 position_to_focus)
{
	current_time += App->time->delta_time;
	float focus_progress = math::Min((current_time - start_focus_time) / 150.f, 1.f);
	float3 new_camera_position = owner->transform.GetTranslation().Lerp(position_to_focus, focus_progress);
	owner->transform.SetTranslation(float3(new_camera_position.x, position_to_focus.y, new_camera_position.z));
	is_focusing = focus_progress != 1;

}

void CameraController::FollowPlayer() 
{
	distance_x = abs(player1->transform.GetTranslation().x - owner->transform.GetTranslation().x);
	if ( distance_x > 2.f && !is_focusing)
	{
		start_focus_time = App->time->delta_time;
		current_time = start_focus_time;
		is_focusing = true;
	}

	distance_z = abs(player1->transform.GetTranslation().z);
	if (distance_z > 1.f && !is_focusing)
	{
		start_focus_time = App->time->delta_time;
		current_time = start_focus_time;
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

void CameraController::SetFreeze()
{
	freeze != freeze;
}

void CameraController::InitPublicGameObjects()
{
	//IMPORTANT, public gameobjects, name_gameobjects and go_uuids MUST have same size
	public_gameobjects.push_back(&player1);
	public_gameobjects.push_back(&player2);

	variable_names.push_back(GET_VARIABLE_NAME(player1));
	variable_names.push_back(GET_VARIABLE_NAME(player2));

	for (unsigned int i = 0; i < public_gameobjects.size(); ++i)
	{
		name_gameobjects.push_back(is_object);
		go_uuids.push_back(0);
	}
}