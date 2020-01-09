#include "ModuleEditor.h"
#include "Globals.h"
#include "Application.h"
#include "ModuleDebug.h"
#include "ModuleFileSystem.h"
#include "ModuleRender.h"
#include "ModuleScene.h"
#include "Config.h"

#include "OLQuadTree.h"
#include "UI/DebugDraw.h"

// Called before render is available
bool ModuleEditor::Init()
{
	APP_LOG_SECTION("************ Module Editor Init ************");
	
	
	APP_LOG_SUCCESS("IMGUI editor initialized correctly.");

	return true;
}

// Called before quitting
bool ModuleEditor::CleanUp()
{
	remove(TMP_SCENE_PATH);

	return true;
}

void ModuleEditor::RenderEditorTools() const
{
	if (App->debug->show_grid)
	{
		dd::xzSquareGrid(-100.0f, 100.0f, 0.0f, 1.0f, math::float3(0.65f, 0.65f, 0.65f));
	}
	if (App->debug->show_camera_frustum && App->scene->hierarchy.selected_game_object != nullptr)
	{
		Component * selected_camera_component = App->scene->hierarchy.selected_game_object->GetComponent(Component::ComponentType::CAMERA);
		if (selected_camera_component != nullptr) {
			ComponentCamera* selected_camera = static_cast<ComponentCamera*>(selected_camera_component);
			dd::frustum(selected_camera->GetInverseClipMatrix(), float3::one);
		}
	}
	if (App->debug->show_quadtree)
	{
		for (auto& ol_quadtree_node : App->renderer->ol_quadtree.flattened_tree)
		{
			float3 quadtree_node_min = float3(ol_quadtree_node->box.minPoint.x, 0, ol_quadtree_node->box.minPoint.y);
			float3 quadtree_node_max = float3(ol_quadtree_node->box.maxPoint.x, 0, ol_quadtree_node->box.maxPoint.y);
			dd::aabb(quadtree_node_min, quadtree_node_max, float3::one);
		}
	}

}

void ModuleEditor::OpenScene(const std::string &path) const
{
	size_t readed_bytes;
	std::string serialized_scene_string = App->filesystem->Load(path.c_str(), readed_bytes);
	
	Config scene_config(serialized_scene_string);
	App->scene->Load(scene_config);
}

void ModuleEditor::SaveScene(const std::string &path) const
{
	Config scene_config;
	App->scene->Save(scene_config);
	std::string serialized_scene_string;
	scene_config.GetSerializedString(serialized_scene_string);

	App->filesystem->Save(path.c_str(), serialized_scene_string.c_str(), serialized_scene_string.size() + 1);
}
