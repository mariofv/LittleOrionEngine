#ifndef _PANELSCENE_H_
#define _PANELSCENE_H_

#include "Panel.h"
#include <MathGeoLib.h>

class Billboard;

class PanelScene : public Panel
{
public:
	PanelScene();
	~PanelScene();

	void Render() override;
	void RenderDebugDraws();

	void MousePicking(const float2& mouse_position);
	void SceneDropTarget();

private:
	void RenderSceneBar();

	void RenderEditorDraws();

	void RenderCameraFrustum() const;
	void RenderOutline() const;
	void RenderBoundingBoxes() const;
	void RenderGlobalBoundingBoxes() const;
	void RenderBillboards() const;

	void RenderGizmo();
	void RenderSceneCameraGizmo() const;
	void RenderCameraPreview() const;

private:
	bool scene_camera_gizmo_hovered = false;

	float scene_window_content_area_width = 0;
	float scene_window_content_area_height = 0;

	float2 scene_window_content_area_pos = float2::zero;

	Billboard* camera_billboard = nullptr;
	Billboard* light_billboard = nullptr;
};

#endif //_PANELSCENE_H_