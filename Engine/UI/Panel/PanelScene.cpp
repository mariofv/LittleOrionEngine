#include "PanelScene.h"

#include "Component/ComponentCamera.h"
#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleCamera.h"
#include "Module/ModuleEditor.h"
#include "Module/ModuleActions.h"
#include "Module/ModuleFileSystem.h"
#include "Module/ModuleModelLoader.h"
#include "Module/ModuleProgram.h"
#include "Module/ModuleRender.h"
#include "Module/ModuleScene.h"
#include "UI/Panel/PanelHierarchy.h"

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

		App->cameras->scene_camera->RecordFrame(scene_window_content_area_width, scene_window_content_area_height);
		App->cameras->scene_camera->RecordDebugDraws(scene_window_content_area_width, scene_window_content_area_height);
		
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
		hovered = content_area.Contains(mouse_pos); // TODO: This seems to be inneficient, check with partner

		RenderEditorDraws(); // This should be render after rendering framebuffer texture.

		if (App->cameras->IsMovementEnabled() && hovered) // CHANGES CURSOR IF SCENE CAMERA MOVEMENT IS ENABLED
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
		ImGui::EndMenuBar();
	}
}


void PanelScene::RenderEditorDraws()
{
	ImGuizmo::SetRect(scene_window_content_area_pos.x, scene_window_content_area_pos.y, scene_window_content_area_width, scene_window_content_area_height);
	ImGuizmo::SetDrawlist();
	ImGuizmo::SetOrthographic(false);
	ImGuizmo::Enable(true);

	if (App->editor->selected_game_object != nullptr)
	{
		RenderGizmo();
		RenderCameraPreview();
	}

	RenderSceneCameraGizmo();
}


void PanelScene::RenderGizmo()
{
	float4x4 model_global_matrix_transposed = App->editor->selected_game_object->transform.GetGlobalModelMatrix().Transposed();

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

		App->editor->selected_game_object->transform.SetGlobalModelMatrix(model_global_matrix_transposed.Transposed());
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

void PanelScene::MousePicking(const float2& mouse_position)
{
	if (scene_camera_gizmo_hovered)
	{
		return;
	}

	float2 window_center_pos = scene_window_content_area_pos + float2(scene_window_content_area_width, scene_window_content_area_height) / 2;

	float2 window_mouse_position = mouse_position - window_center_pos;
	float2 window_mouse_position_normalized = float2(window_mouse_position.x * 2 / scene_window_content_area_width, -window_mouse_position.y * 2 / scene_window_content_area_height);

	LineSegment ray;
	App->cameras->scene_camera->GetRay(window_mouse_position_normalized, ray);
	GameObject* intersected = App->renderer->GetRaycastIntertectedObject(ray);
	App->editor->selected_game_object = intersected;
}


void PanelScene::SceneDropTarget()
{
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_File"))
		{
			assert(payload->DataSize == sizeof(File*));
			File *incoming_file = *(File**)payload->Data;
			if (incoming_file->file_type == FileType::MODEL)
			{
				GameObject* new_model = App->model_loader->LoadModel(incoming_file->file_path.c_str());
				App->scene->root->AddChild(new_model); 
				
				App->actions->action_game_object = new_model;
				App->actions->AddUndoAction(ModuleActions::UndoActionType::ADD_GAMEOBJECT);
			}
		}
		ImGui::EndDragDropTarget();
	}

}
