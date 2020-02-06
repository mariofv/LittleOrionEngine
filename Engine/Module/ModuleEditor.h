#ifndef _MODULEEDITOR_H_
#define _MODULEEDITOR_H_

#include "Module.h"
#include "Globals.h"
#include "Component/ComponentCamera.h"
#include "UI/Billboard.h"
#include "Actions/EditorAction.h"
#include <stack>

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

	void ShowSceneTab();
	void ShowGameTab();

	void ShowGizmoControls();

	void ClearRedoStack();
	void Undo();
	void Redo();

	void AddUndoAction(int type);

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
	bool clicked = false;
	int type_of_action = 0;
	std::stack<EditorAction*> undoStack;
	std::stack<EditorAction*> redoStack;
	float3 previous_transform = float3::zero;

private:
	float scene_window_content_area_width = 0;
	float scene_window_content_area_height = 0;

	float2 scene_window_content_area_pos = float2::zero;

	ImGuizmo::OPERATION gizmo_operation = ImGuizmo::TRANSLATE;
	bool gizmo_hovered = false;
	bool gizmo_released = false;


	Billboard* camera_billboard = nullptr;
	Billboard* light_billboard = nullptr;

};

#endif //_MODULEEDITOR_H_