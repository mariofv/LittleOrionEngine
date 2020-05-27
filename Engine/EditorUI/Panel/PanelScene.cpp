#include "PanelScene.h"

#include "Component/ComponentCamera.h"
#include "EditorUI/Panel/PanelHierarchy.h"
#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleAI.h"
#include "Module/ModuleCamera.h"
#include "Module/ModuleDebug.h"
#include "Module/ModuleEditor.h"
#include "Module/ModuleActions.h"
#include "Module/ModuleFileSystem.h"
#include "Module/ModuleResourceManager.h"
#include "Module/ModuleProgram.h"
#include "Module/ModuleRender.h"
#include "Module/ModuleScene.h"
#include "Module/ModuleTime.h"

#include "ResourceManagement/Importer/Importer.h"
#include "ResourceManagement/Resources/Prefab.h"

#include <Brofiler/Brofiler.h>
#include <imgui.h>
#include <FontAwesome5/IconsFontAwesome5.h>

PanelScene::PanelScene()
{
	opened = true;
	enabled = true;
	window_name = ICON_FA_TH " Scene";
}


PanelScene::~PanelScene()
{
}

void PanelScene::Render()
{
	BROFILER_CATEGORY("Render Scene Panel", Profiler::Color::BlueViolet);

	if (ImGui::Begin(ICON_FA_TH " Scene", &opened, ImGuiWindowFlags_MenuBar))
	{
		RenderSceneBar();

		ImVec2 scene_window_pos_ImVec2 = ImGui::GetWindowPos();
		float2 scene_window_pos = float2(scene_window_pos_ImVec2.x, scene_window_pos_ImVec2.y);

		ImVec2 scene_window_content_area_max_point_ImVec2 = ImGui::GetWindowContentRegionMax();
		scene_window_content_area_max_point_ImVec2 = ImVec2(
			scene_window_content_area_max_point_ImVec2.x + scene_window_pos_ImVec2.x,
			scene_window_content_area_max_point_ImVec2.y + scene_window_pos_ImVec2.y
		); // Pass from window space to screen space
		float2 scene_window_content_area_max_point = float2(scene_window_content_area_max_point_ImVec2.x, scene_window_content_area_max_point_ImVec2.y);

		ImVec2 scene_window_content_area_pos_ImVec2 = ImGui::GetCursorScreenPos();
		scene_window_content_area_pos = float2(scene_window_content_area_pos_ImVec2.x, scene_window_content_area_pos_ImVec2.y);

		scene_window_content_area_width = scene_window_content_area_max_point.x - scene_window_content_area_pos.x;
		scene_window_content_area_height = scene_window_content_area_max_point.y - scene_window_content_area_pos.y;

		App->cameras->scene_camera->RecordFrame(scene_window_content_area_width, scene_window_content_area_height, true);
		App->debug->Render(App->cameras->scene_camera);
		App->cameras->scene_camera->RecordDebugDraws(true);

		ImGui::Image(
			(void *)App->cameras->scene_camera->GetLastRecordedFrame(),
			ImVec2(scene_window_content_area_width, scene_window_content_area_height),
			ImVec2(0, 1),
			ImVec2(1, 0)
		);
		SceneDropTarget();

		AABB2D content_area = AABB2D(scene_window_content_area_pos, scene_window_content_area_max_point);
		float2 mouse_pos_f2 = float2(ImGui::GetMousePos().x, ImGui::GetMousePos().y);
		AABB2D mouse_pos = AABB2D(mouse_pos_f2, mouse_pos_f2);
		hovered = ImGui::IsWindowHovered(); // TODO: This seems to be inneficient, check with partner
		focused = ImGui::IsWindowFocused();

		RenderEditorDraws(); // This should be render after rendering framebuffer texture.

		if (App->cameras->IsSceneCameraMoving()) // CHANGES CURSOR IF SCENE CAMERA MOVEMENT IS ENABLED
		{
			ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);
		}
	}
	ImGui::End();
}

void PanelScene::RenderSceneBar()
{
	if (ImGui::BeginMenuBar())
	{
		std::string draw_mode = App->renderer->GetDrawMode();

		if (ImGui::BeginMenu((draw_mode + " " ICON_FA_CARET_DOWN).c_str()))
		{
			if (ImGui::MenuItem("Shaded", NULL, draw_mode == "Shaded"))
			{
				App->renderer->SetDrawMode(ModuleRender::DrawMode::SHADED);
			}
			if (ImGui::MenuItem("Wireframe", NULL, draw_mode == "Wireframe"))
			{
				App->renderer->SetDrawMode(ModuleRender::DrawMode::WIREFRAME);
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu(ICON_FA_VIDEO " " ICON_FA_CARET_DOWN))
		{
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Scene Camera");
			if (ImGui::SliderFloat("Field of View", &App->cameras->scene_camera->camera_frustum.verticalFov, 0, 3.14f))
			{
				App->cameras->scene_camera->SetFOV(App->cameras->scene_camera->camera_frustum.verticalFov);
			}

			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Navigation");
			ImGui::DragFloat("Camera Speed", &App->cameras->scene_camera->camera_movement_speed, 0.1f, 0, 10.f);
			
			ImGui::EndMenu();
		}

		if (ImGui::Selectable("Stats", App->debug->show_debug_metrics, ImGuiSelectableFlags_None, ImVec2(40,0)))
		{
			App->debug->show_debug_metrics = !App->debug->show_debug_metrics;
		}

		ImGui::EndMenuBar();

	}
}


void PanelScene::RenderEditorDraws()
{
	BROFILER_CATEGORY("Render Editor Draws", Profiler::Color::Lavender);

	ImGuizmo::SetRect(scene_window_content_area_pos.x, scene_window_content_area_pos.y, scene_window_content_area_width, scene_window_content_area_height);
	ImGuizmo::SetDrawlist();
	ImGuizmo::SetOrthographic(false);
	ImGuizmo::Enable(true);

	if (App->editor->selected_game_object != nullptr)
	{
		RenderGizmo();
		RenderCameraPreview();
	}

	if (App->debug->show_debug_metrics)
	{
		RenderDebugMetrics();
	}

	RenderSceneCameraGizmo();

}


void PanelScene::RenderGizmo()
{
	ComponentTransform* selected_object_transform = nullptr;
	if (App->editor->selected_game_object->GetTransformType() == Component::ComponentType::TRANSFORM)
	{
		selected_object_transform = &App->editor->selected_game_object->transform;
	}
	else
	{
		selected_object_transform = &App->editor->selected_game_object->transform_2d;
	}
	float4x4 model_global_matrix_transposed = selected_object_transform->GetGlobalModelMatrix().Transposed();

	if (!gizmo_released && !App->actions->clicked)
	{
		//Save current position/rotation/scale of transform depending on operation
		switch (App->editor->gizmo_operation)
		{
		case ImGuizmo::TRANSLATE:
			App->actions->previous_transform = App->editor->selected_game_object->transform.GetTranslation();
			break;
		case ImGuizmo::ROTATE:
			App->actions->previous_transform = App->editor->selected_game_object->transform.GetRotationRadiants();
			break;
		case ImGuizmo::SCALE:
			App->actions->previous_transform = App->editor->selected_game_object->transform.GetScale();
			break;
		case ImGuizmo::BOUNDS:
			break;
		default:
			break;
		}
	}


	ImGuizmo::Manipulate(
		App->cameras->scene_camera->GetViewMatrix().Transposed().ptr(),
		App->cameras->scene_camera->GetProjectionMatrix().Transposed().ptr(),
		App->editor->gizmo_operation,
		ImGuizmo::LOCAL,
		model_global_matrix_transposed.ptr()
	);

	scene_camera_gizmo_hovered = ImGuizmo::IsOver();
	if (ImGuizmo::IsUsing())
	{
		gizmo_released = true;
		selected_object_transform->SetGlobalModelMatrix(model_global_matrix_transposed.Transposed());
		selected_object_transform->modified_by_user = true;
	}
	else if (gizmo_released)
	{
		//Guizmo have been released so an actionTransform have been done
		ModuleActions::UndoActionType action_type;
		switch (App->editor->gizmo_operation)
		{
		case ImGuizmo::TRANSLATE:
			action_type = ModuleActions::UndoActionType::TRANSLATION;
			break;

		case ImGuizmo::ROTATE:
			action_type = ModuleActions::UndoActionType::ROTATION;
			break;

		case ImGuizmo::SCALE:
			action_type = ModuleActions::UndoActionType::SCALE;
			break;

		default:
			break;
		}

		App->actions->AddUndoAction(action_type);
		gizmo_released = false;
	}
}

void PanelScene::RenderSceneCameraGizmo() const
{
	float4x4 old_view_matrix = App->cameras->scene_camera->GetViewMatrix().Transposed();
	float4x4 transposed_view_matrix = App->cameras->scene_camera->GetViewMatrix().Transposed();
	bool modified;
	ImGuizmo::ViewManipulate(
		transposed_view_matrix.ptr(),
		1,
		ImVec2(scene_window_content_area_pos.x + scene_window_content_area_width - 100, scene_window_content_area_pos.y),
		ImVec2(100, 100),
		ImGui::ColorConvertFloat4ToU32(ImVec4(1.f, 1.f, 1.f, 0.f)),
		modified
	);

	if (modified)
	{
		App->cameras->scene_camera->SetViewMatrix(transposed_view_matrix.Transposed());
	}

}

void PanelScene::RenderCameraPreview() const
{
	Component * selected_camera_component = App->editor->selected_game_object->GetComponent(Component::ComponentType::CAMERA);
	if (selected_camera_component != nullptr) {
		ComponentCamera* selected_camera = static_cast<ComponentCamera*>(selected_camera_component);

		ImGui::SetCursorPos(ImVec2(scene_window_content_area_width - 200, scene_window_content_area_height - 200));
		ImGui::BeginChildFrame(ImGui::GetID("Camera Preview"), ImVec2(200, 200), ImGuiWindowFlags_MenuBar);
		if (ImGui::BeginMenuBar())
		{
			ImGui::BeginMenu("Camera Preview", false);
			ImGui::EndMenuBar();
		}

		ImVec2 content_area_max_point = ImGui::GetWindowContentRegionMax();

		float width = content_area_max_point.x - ImGui::GetCursorPos().x;
		float height = content_area_max_point.y - ImGui::GetCursorPos().y;

		selected_camera->RecordFrame(width, height);
		ImGui::Image(
			(void *)selected_camera->GetLastRecordedFrame(),
			ImVec2(width, height),
			ImVec2(0, 1),
			ImVec2(1, 0)
		);

		ImGui::EndChild();
	}
}

void PanelScene::RenderDebugMetrics() const
{
	ImGui::SetCursorPos(ImVec2(10, 50));
	ImGui::BeginChildFrame(ImGui::GetID("Debug Metrics"), ImVec2(200, 100), ImGuiWindowFlags_MenuBar);
	if (ImGui::BeginMenuBar())
	{
		ImGui::BeginMenu("Debug Metrics", false);
		ImGui::EndMenuBar();
	}

	ImGui::Text("FPS: %f.2", App->time->GetFPS());
	ImGui::Text("Tris: %d", App->renderer->GetRenderedTris());

	ImGui::EndChild();
}

void PanelScene::MousePicking(const float2& mouse_position)
{
	if (scene_camera_gizmo_hovered)
	{
		return;
	}

	LineSegment ray;
	App->cameras->scene_camera->GetRay(mouse_position, ray);
	RaycastHit* hit = App->renderer->GetRaycastIntersection(ray, App->cameras->scene_camera);
	App->editor->selected_game_object = hit->game_object;
	delete(hit);
}


void PanelScene::SceneDropTarget()
{
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_Resource"))
		{
			assert(payload->DataSize == sizeof(Metafile*));
			Metafile* incoming_metafile = *((Metafile**)payload->Data);
			if (incoming_metafile->resource_type == ResourceType::PREFAB)
			{
				std::shared_ptr<Prefab> prefab = App->resources->Load<Prefab>(incoming_metafile->uuid);
				GameObject* new_model = prefab->Instantiate(App->scene->root);
				App->actions->action_game_object = new_model;
				App->actions->AddUndoAction(ModuleActions::UndoActionType::ADD_GAMEOBJECT);
			}

			if (incoming_metafile->resource_type == ResourceType::MODEL)
			{
				std::shared_ptr<Prefab> prefab = App->resources->Load<Prefab>(incoming_metafile->uuid);
				prefab->overwritable = false;

				GameObject* new_model = prefab->Instantiate(App->scene->root);
				App->actions->action_game_object = new_model;
				App->actions->AddUndoAction(ModuleActions::UndoActionType::ADD_GAMEOBJECT);
			}
		}
		ImGui::EndDragDropTarget();
	}

}
