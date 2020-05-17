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
#include "Module/ModuleDebug.h"
#include "Module/ModuleSpacePartitioning.h"

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
}

// Use this for initialization
void DebugModeScript::Start()
{

}

// Update is called once per frame
void DebugModeScript::Update()
{
	if (App->input->GetKeyDown(KeyCode::F1) || App->input->GetControllerButtonDown(ControllerCode::RightStick, ControllerID::ONE))
	{
		debug_enabled = !debug_enabled;

		if (debug_enabled)
		{
			if(render_AABB)
			{
				App->space_partitioning->DrawAABBTree();
			}
		}
	}
}

void DebugModeScript::UpdateWithImGui(ImGuiContext* context)
{
	//Necessary to be able to write with imgui
	ImGui::SetCurrentContext(context);

	if (debug_enabled)
	{
		if (ImGui::Begin("Ingame debug (Shhhhhh!! This is a secret!)"))
		{
			std::stringstream stream_fps;
			std::stringstream stream_ms;
			stream_fps << std::fixed << std::setprecision(2) << App->time->GetFPS();
			stream_ms << std::fixed << std::setprecision(2) << App->time->delta_time;

			ImGui::LabelText((stream_fps.str() + std::string(" (") + stream_ms.str() + std::string("ms)")).c_str(), base_str_fps.c_str());
			ImGui::LabelText(std::to_string(App->renderer->GetRenderedTris()).c_str(), base_str_tris.c_str());
			ImGui::LabelText(std::to_string(App->renderer->GetRenderedVerts()).c_str(), base_str_verts.c_str());

			ImGui::Separator();

			if (ImGui::Checkbox("Draw Wireframe? ", &render_wireframe))
			{
				App->renderer->SetDrawMode(render_wireframe ? ModuleRender::DrawMode::WIREFRAME : ModuleRender::DrawMode::SHADED);
			}

			ImGui::Checkbox("Draw AABB? ", &render_AABB);
			ImGui::Checkbox("Toggle Invincible mode ", &is_player_invincible);

			ImGui::End();
		}
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

