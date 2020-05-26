#include "DebugModeScript.h"

#include "Component/ComponentCamera.h"
#include "Component/ComponentCapsuleCollider.h"
#include "Component/ComponentImage.h"
#include "Component/ComponentScript.h"
#include "Component/ComponentText.h"
#include "Component/ComponentTransform.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleCamera.h"
#include "Module/ModuleDebug.h"
#include "Module/ModuleDebugDraw.h"
#include "Module/ModuleEditor.h"
#include "Module/ModuleInput.h"
#include "Module/ModulePhysics.h"
#include "Module/ModuleRender.h"
#include "Module/ModuleScene.h"
#include "Module/ModuleSpacePartitioning.h"
#include "Module/ModuleTime.h"

#include "EditorUI/Panel/InspectorSubpanel/PanelComponent.h"

#include "EnemyManager.h"
#include "SceneCamerasController.h"

#include "imgui.h"
#include <imgui_internal.h>
#include <imgui_stdlib.h>
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
	//Necessary to be able to write with imgui
	//ImGui::SetCurrentContext(ImGui::GetCurrentContext());

	if (App->input->GetKeyDown(KeyCode::F1) || App->input->GetControllerButtonDown(ControllerCode::RightStick, ControllerID::ONE))
	{
		debug_enabled = !debug_enabled;
	}

	if (debug_enabled)
	{
		ImGui::SetCurrentContext(App->editor->GetImGuiContext());

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

			ImGui::Checkbox("Draw AABBTree? ", &show_aabbtree);
			if (show_aabbtree)
			{
				App->debug_draw->RenderAABBTree();
			}

			ImGui::Checkbox("Draw Navmesh? ", &show_navmesh);
			if (show_navmesh)
			{
				App->debug_draw->RenderNavMesh(*App->cameras->main_camera);
			}

			ImGui::Checkbox("Draw QuadTree? ", &show_quadtree);
			if (show_quadtree)
			{
				App->debug_draw->RenderQuadTree();
			}

			ImGui::Checkbox("Draw OctTree? ", &show_octtree);
			if (show_octtree)
			{
				App->debug_draw->RenderOcTree();
			}

			ImGui::Checkbox("Draw Bounding boxes? ", &show_bounding_boxes);
			if (show_bounding_boxes)
			{
				App->debug_draw->RenderBoundingBoxes();
			}

			ImGui::Checkbox("Draw Global bounding boxes? ", &show_global_bounding_boxes);
			if (show_global_bounding_boxes)
			{
				App->debug_draw->RenderGlobalBoundingBoxes();
			}

			ImGui::Checkbox("Draw Pathfinding? ", &show_pathfind_points);
			if (show_pathfind_points)
			{
				App->debug_draw->RenderPathfinding();
			}

			if(ImGui::Checkbox("Draw Physics? ", &show_physics))
			{
				App->physics->show_physics = show_physics;
			}
			
			ImGui::Checkbox("Toggle Invincible mode ", &is_player_invincible);
			
			if (is_spawning_enemy)
			{
				ImGui::Text("Please touch anywhere to spawn the enemy");
			}
			else if(ImGui::Button("Spawn enemy"))
			{
				is_spawning_enemy = true;
			}

			if (is_warping_player_one || is_warping_player_two)
			{
				ImGui::Text("Please touch somewhere to warp the character");
			}
			else if(!has_warped_player_recently)
			{
				if (ImGui::Button("Warp Player One"))
				{
					is_warping_player_one = true;
				}
				ImGui::SameLine();
				if (ImGui::Button("Warp Player Two"))
				{
					is_warping_player_two = true;
				}
			}


			ImGui::LabelText(last_touched_game_object, "Last touched gameobject");
			ImGui::End();
		}

		if (App->input->GetMouseButtonDown(MouseButton::Left))
		{
			LineSegment ray;
			App->cameras->main_camera->GetRay(App->input->GetMousePosition(), ray);
			RaycastHit* hit = App->renderer->GetRaycastIntersection(ray, App->cameras->main_camera);

			if (hit->game_object != nullptr)
			{
				if (is_warping_player_one || is_warping_player_two)
				{
					ComponentCapsuleCollider* collider = nullptr;

					if (is_warping_player_one) collider = (ComponentCapsuleCollider*)player_one_obj->GetComponent(ComponentCollider::ColliderType::CAPSULE);
					else collider = (ComponentCapsuleCollider*)player_two_obj->GetComponent(ComponentCollider::ColliderType::CAPSULE);

					collider->active_physics = false;

					if (is_warping_player_one) player_one_obj->transform.SetTranslation(hit->hit_point);
					else player_two_obj->transform.SetTranslation(hit->hit_point);

					is_warping_player_one = false;
					is_warping_player_two = false;
					has_warped_player_recently = true;
				}

				if (is_spawning_enemy)
				{
					LineSegment ray;
					App->cameras->main_camera->GetRay(App->input->GetMousePosition(), ray);
					enemy_manager->SpawnEnemy(0, hit->hit_point);
					is_spawning_enemy = false;
				}

				last_touched_game_object = hit->game_object->name.c_str();
			}
		}

		if (has_warped_player_recently)
		{
			warp_cooldown += (App->time->delta_time / 1000);
			if (warp_cooldown >= 1.5f)//Half a second cooldown
			{
				warp_cooldown = 0.0f;
				has_warped_player_recently = false;
				ComponentCapsuleCollider* collider = (ComponentCapsuleCollider*)player_one_obj->GetComponent(ComponentCollider::ColliderType::CAPSULE);
				collider->active_physics = true;
			}

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
	
	public_gameobjects.push_back(&player_one_obj);
	variable_names.push_back(GET_VARIABLE_NAME(player_one_obj));
	
	public_gameobjects.push_back(&player_two_obj);
	variable_names.push_back(GET_VARIABLE_NAME(player_two_obj));

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
