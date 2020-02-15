#ifndef _MODULEEDITOR_H_
#define _MODULEEDITOR_H_

#include "Module.h"
#include "Main/Globals.h"
#include "Component/ComponentCamera.h"
#include "UI/Billboard.h"
#include "Actions/EditorAction.h"

#include <vector>

#include <ImGuizmo.h>

class ModuleEditor : public Module
{
public:
	ModuleEditor() = default;
	~ModuleEditor() = default;

	bool Init() override;
	update_status Update() override;
	bool CleanUp() override;

	void OpenScene(const std::string &path) const;
	void SaveScene(const std::string &path) const;

	void RenderDebugDraws();

	void MousePicking(const float2& mouse_position);
	GameObject* GetRaycastIntertectedObject(const LineSegment & ray);

	void ShowSceneTab();
	void ShowGameTab();

	void ShowGizmoControls();

	void ClearRedoStack();
	void ClearUndoStack();
	void Undo();
	void Redo();

	void AddUndoAction(const int type);
	void DeleteComponentUndo(Component* comp);
	void ClearUndoRedoStacks();

private:
	void RenderCameraFrustum() const;
	void RenderOutline() const;
	void RenderBoundingBoxes() const;
	void RenderGlobalBoundingBoxes() const;
	void RenderBillboards() const;

	void RenderEditorDraws();
	void RenderGizmo();
	void RenderEditorCameraGizmo() const;
	void RenderCameraPreview() const;

	void SceneDropTarget();

public:
	bool scene_window_is_hovered = false;
	//UndoRedo
	bool clicked = false;
	bool clicked_light = false;
	bool clicked_camera = false;
	std::vector<EditorAction*> undoStack;
	std::vector<EditorAction*> redoStack;
	float3 previous_transform = float3::zero;
	GameObject* action_game_object = nullptr;
	Component* action_component = nullptr;


	float previous_light_color[3];
	float previous_light_intensity;

	Texture::TextureType type_texture;



private:
	float scene_window_content_area_width = 0;
	float scene_window_content_area_height = 0;

	float2 scene_window_content_area_pos = float2::zero;
	const int MAXIMUM_SIZE_STACK_UNDO = 5;

	ImGuizmo::OPERATION gizmo_operation = ImGuizmo::TRANSLATE;
	bool gizmo_hovered = false;
	bool gizmo_released = false;


	Billboard* camera_billboard = nullptr;
	Billboard* light_billboard = nullptr;

};

#endif //_MODULEEDITOR_H_