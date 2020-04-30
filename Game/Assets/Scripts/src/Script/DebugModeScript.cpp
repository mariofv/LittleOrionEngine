#include "DebugModeScript.h"

#include "Component/ComponentImage.h"
#include "Component/ComponentScript.h"
#include "Component/ComponentTransform.h"
#include "Component/ComponentText.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleRender.h"
#include "Module/ModuleScene.h"
#include "Module/ModuleTime.h"

#include "EditorUI/Panel/InspectorSubpanel/PanelComponent.h"

#include "SceneCamerasController.h"

#include "imgui.h"
#include <iomanip>
#include <sstream>



DebugModeScript* DebugModeScriptDLL()
{
	DebugModeScript* instance = new DebugModeScript();
	return instance;
}

DebugModeScript::DebugModeScript()
{
	panel = new PanelComponent();
}

// Use this for initialization before Start()
void DebugModeScript::Awake()
{
	background = (ComponentImage*)background_go->GetComponent(ComponentUI::UIType::IMAGE);
	text_fps = (ComponentText*)text_fps_go->GetComponent(ComponentUI::UIType::TEXT);
	text_tris = (ComponentText*)text_tris_go->GetComponent(ComponentUI::UIType::TEXT);
	text_verts = (ComponentText*)text_verts_go->GetComponent(ComponentUI::UIType::TEXT);

	text_fps->scale = 8.0f;
	text_tris->scale = 8.0f;
	text_verts->scale = 8.0f;
	text_fps->color = float3::zero;
	text_tris->color = float3::zero;
	text_verts->color = float3::zero;

	ComponentScript* component = camera_manager->GetComponentScript("SceneCamerasController");
	scene_cameras = (SceneCamerasController*)component->script;
}

// Use this for initialization
void DebugModeScript::Start()
{

}

// Update is called once per frame
void DebugModeScript::Update()
{

	if(App->input->GetKeyDown(KeyCode::F1) || App->input->GetControllerButtonDown(ControllerCode::RightStick))
	{
		(debug_enabled) ? text_fps->Disable() : text_fps->Enable();
		(debug_enabled) ? text_tris->Disable() : text_tris->Enable();
		(debug_enabled) ? text_verts->Disable() : text_verts->Enable();
		(debug_enabled) ? background->Disable() : background->Enable();
		(debug_enabled) ? scene_cameras->SetMainCameraRendering() : scene_cameras->SetMainCameraRendering();
		debug_enabled = !debug_enabled;
		
		
	}

	if (debug_enabled)
	{
		std::stringstream stream_fps;
		std::stringstream stream_ms;
		stream_fps << std::fixed << std::setprecision(2) << App->time->GetFPS();
		stream_ms << std::fixed << std::setprecision(2) << App->time->delta_time;
		text_fps->text = base_str_fps + stream_fps.str()  + std::string(" (") + stream_ms.str() + std::string("ms)");
		text_tris->text = base_str_tris + std::to_string(App->renderer->GetRenderedTris());
		text_verts->text = base_str_verts + std::to_string(App->renderer->GetRenderedVerts());
	}

}

// Use this for showing variables on inspector
void DebugModeScript::OnInspector(ImGuiContext* context)
{
	//Necessary to be able to write with imgui
	ImGui::SetCurrentContext(context);
	ShowDraggedObjects();

}

//Use this for linking JUST GO automatically 
void DebugModeScript::InitPublicGameObjects()
{
	//IMPORTANT, public gameobjects, name_gameobjects and go_uuids MUST have same size

	public_gameobjects.push_back(&background_go);
	variable_names.push_back(GET_VARIABLE_NAME(background_go));

	public_gameobjects.push_back(&text_tris_go);
	variable_names.push_back(GET_VARIABLE_NAME(text_tris_go));

	public_gameobjects.push_back(&text_verts_go);
	variable_names.push_back(GET_VARIABLE_NAME(text_verts_go));

	public_gameobjects.push_back(&text_fps_go);
	variable_names.push_back(GET_VARIABLE_NAME(text_fps_go));

	public_gameobjects.push_back(&camera_manager);
	variable_names.push_back(GET_VARIABLE_NAME(camera_manager));

	for (unsigned int i = 0; i < public_gameobjects.size(); ++i)
	{
		name_gameobjects.push_back(is_object);
		go_uuids.push_back(0);
	}
}
//Use this for linking GO AND VARIABLES automatically if you need to save variables 
// void DebugModeScript::Save(Config& config) const
// {
// 	config.AddUInt(example->UUID, "ExampleNameforSave");
// 	Script::Save(config);
// }

// //Use this for linking GO AND VARIABLES automatically
// void DebugModeScript::Load(const Config& config)
// {
// 	exampleUUID = config.GetUInt("ExampleNameforSave", 0);
// 	Script::Load(config);
// }

