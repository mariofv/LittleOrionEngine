#include "DebugModeScript.h"

#include "Component/ComponentImage.h"
#include "Component/ComponentScript.h"
#include "Component/ComponentTransform.h"
#include "Component/ComponentText.h"
#include "Component/ComponentCamera.h"
#include "Component/ComponentCapsuleCollider.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleRender.h"
#include "Module/ModuleScene.h"
#include "Module/ModuleTime.h"
#include "Module/ModuleDebug.h"
#include "Module/ModuleSpacePartitioning.h"
#include "Module/ModuleCamera.h"

#include "EditorUI/Panel/InspectorSubpanel/PanelComponent.h"

#include "SceneCamerasController.h"
#include "EnemyManager.h"

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
	const ComponentScript* component = enemy_manager_obj->GetComponentScript("EnemyManager");
	enemy_manager = (EnemyManager*)component->script;
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
	}

	if (debug_enabled)
	{
		if (is_warping_player)
		{
			if (App->input->GetMouseButtonDown(MouseButton::Left))
			{
				LineSegment ray;
				App->cameras->main_camera->GetRay(App->input->GetMousePosition(), ray);
				RaycastHit* hit = App->renderer->GetRaycastIntertectedObject(ray, App->cameras->main_camera);

				if (hit->game_object != nullptr)
				{
					ComponentCapsuleCollider* collider = (ComponentCapsuleCollider*)player_obj->GetComponent(ComponentCollider::ColliderType::CAPSULE);
					
					collider->active_physics = false;
					player_obj->transform.SetTranslation(hit->hit_point);
					is_warping_player = false;
					has_warped_player_recently = true;
				}
			}
		}

		if (has_warped_player_recently)
		{
			warp_cooldown += (App->time->delta_time / 1000);
			if (warp_cooldown >= 2.5f)//Half a second cooldown
			{
				warp_cooldown = 0.0f;
				has_warped_player_recently = false;
				ComponentCapsuleCollider* collider = (ComponentCapsuleCollider*)player_obj->GetComponent(ComponentCollider::ColliderType::CAPSULE);
				collider->active_physics = true;
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

			ImGui::Checkbox("Draw AABBTree? ", &App->debug->show_aabbtree);
			ImGui::Checkbox("Draw Navmesh? ", &App->debug->show_navmesh);
			ImGui::Checkbox("Draw QuadTree? ", &App->debug->show_quadtree);
			ImGui::Checkbox("Draw OctTree? ", &App->debug->show_octtree);
			ImGui::Checkbox("Draw Bounding boxes? ", &App->debug->show_bounding_boxes);
			ImGui::Checkbox("Draw Global bounding boxes? ", &App->debug->show_global_bounding_boxes);
			ImGui::Checkbox("Draw Pathfinding? ", &App->debug->show_pathfind_points);
			
			ImGui::Checkbox("Toggle Invincible mode ", &is_player_invincible);

			if(ImGui::Button("Spawn enemy wave"))
			{
				enemy_manager->SpawnWave(0, 5);
			}

			if (is_warping_player)
			{
				ImGui::LabelText("", "Please touch somewhere to warp the character");
			}
			else if (ImGui::Button("Warp Player"))
			{
				is_warping_player = true;
			}

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
	public_gameobjects.push_back(&enemy_manager_obj);
	variable_names.push_back(GET_VARIABLE_NAME(enemy_manager_obj));
	
	public_gameobjects.push_back(&player_obj);
	variable_names.push_back(GET_VARIABLE_NAME(player_obj));

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

