#include "GodCamera.h"

#include "Component/ComponentCamera.h"
#include "Component/ComponentScript.h"
#include "Component/ComponentTransform.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleScene.h"

#include "EditorUI/Panel/InspectorSubpanel/PanelComponent.h"

#include "imgui.h"

#include "DebugModeScript.h"
#include "CameraController.h"

GodCamera* GodCameraDLL()
{
	GodCamera* instance = new GodCamera();
	return instance;
}



GodCamera::GodCamera()
{
	panel = new PanelComponent();
}



// Use this for initialization before Start()
void GodCamera::Awake()
{


}



// Use this for initialization
void GodCamera::Start()
{
	ComponentScript* component_debug = debug->GetComponentScript("DebugModeScript");
	debug_mode = (DebugModeScript*)component_debug->script;

	game_camera_component = (ComponentCamera*)game_camera->GetComponent(Component::ComponentType::CAMERA);
	ComponentScript* game_camera_script = game_camera->GetComponentScript("CameraController");
	camera_controller = (CameraController*)game_camera_script->script;

	camera_component = (ComponentCamera*)owner->GetComponent(Component::ComponentType::CAMERA);

}

// Update is called once per frame
void GodCamera::Update()
{
	if (!debug_mode->debug_enabled && god_mode)
	{
		owner->transform.SetTranslation(game_camera->transform.GetTranslation());
		owner->transform.SetRotation(game_camera->transform.GetRotation());
		god_mode != god_mode;
		Active();
	}
	if (App->input->GetKeyDown(KeyCode::Alpha1) || App->input->GetControllerButtonDown(ControllerCode::Back, ControllerID::ONE))
	{
		if (debug_mode->debug_enabled || god_mode)
		{
			owner->transform.SetTranslation(game_camera->transform.GetTranslation());
			owner->transform.SetRotation(game_camera->transform.GetRotation());
			god_mode = !god_mode;
			Active();
		}
	}
	if (god_mode && debug_mode->debug_enabled)
	{
		GodModeCamera();
	}

}

void GodCamera::GodModeCamera()
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
		float3 direction = right_axis_direction * rotation_speed + owner->transform.GetTranslation();;
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

void GodCamera::Active()
{
	camera_controller->god_mode = god_mode;
	camera_controller->ActivePlayer();
	if(god_mode)
	{
		camera_component->Enable();
		game_camera_component->Disable();
	}
	else 
	{
		camera_component->Disable();
		game_camera_component->Enable();
	}


}

// Use this for showing variables on inspector
void GodCamera::OnInspector(ImGuiContext* context)
{
	//Necessary to be able to write with imgui
	ImGui::SetCurrentContext(context);
	ShowDraggedObjects();

}


//Use this for linking JUST GO automatically 
void GodCamera::InitPublicGameObjects()
{
	//IMPORTANT, public gameobjects, name_gameobjects and go_uuids MUST have same size
	public_gameobjects.push_back(&game_camera);
	public_gameobjects.push_back(&debug);

	variable_names.push_back(GET_VARIABLE_NAME(game_camera));
	variable_names.push_back(GET_VARIABLE_NAME(debug));


	for (int i = 0; i < public_gameobjects.size(); ++i)
	{
		name_gameobjects.push_back(is_object);
		go_uuids.push_back(0);
	}

}

//Use this for linking GO AND VARIABLES automatically if you need to save variables 
// void GodCamera::Save(Config& config) const
// {
// 	config.AddUInt(example->UUID, "ExampleNameforSave");
// 	Script::Save(config);
// }



// //Use this for linking GO AND VARIABLES automatically
// void GodCamera::Load(const Config& config)
// {
// 	exampleUUID = config.GetUInt("ExampleNameforSave", 0);
// 	Script::Load(config);
// }