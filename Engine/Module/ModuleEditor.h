#ifndef _MODULEEDITOR_H_
#define _MODULEEDITOR_H_

#include "Module.h"
#include "Globals.h"
#include "Component/ComponentCamera.h"

#include <ImGuizmo.h>

class ModuleEditor : public Module
{
public:
	ModuleEditor() = default;
	~ModuleEditor() = default;

	bool Init() override;
	bool CleanUp() override;

	void OpenScene(const std::string &path) const;
	void SaveScene(const std::string &path) const;

	void RenderDebugDraws();

	void MousePicking(const float2& mouse_position);

	void ShowSceneTab();
	void ShowGameTab();

	void ShowGizmoControls();

private:
	void RenderCameraFrustum() const;
	void RenderOutline() const;
	void RenderBoundingBoxes() const;
	void RenderGlobalBoundingBoxes() const;

	void RenderEditorDraws();
	void RenderGizmo();
	void RenderEditorCameraGizmo();
	void RenderCameraPreview() const;

public:
	bool scene_window_is_hovered = false;

private:
	float scene_window_content_area_width = 0;
	float scene_window_content_area_height = 0;

	float2 scene_window_content_area_pos = float2::zero;

	ImGuizmo::OPERATION gizmo_operation = ImGuizmo::TRANSLATE;
	bool gizmo_hovered = false;

};

#endif //_MODULEEDITOR_H_