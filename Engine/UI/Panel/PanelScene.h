#ifndef _PANELSCENE_H_
#define _PANELSCENE_H_

#include "Panel.h"
#include <MathGeoLib.h>

class PanelScene : public Panel
{
public:
	PanelScene();
	~PanelScene();

	void Render() override;

	void MousePicking(const float2& mouse_position);
	void SceneDropTarget();

private:
	void RenderSceneBar();

	void RenderEditorDraws();

	void RenderGizmo();
	void RenderSceneCameraGizmo() const;
	void RenderCameraPreview() const;

private:
	bool scene_camera_gizmo_hovered = false;
	bool gizmo_released = false;

	float scene_window_content_area_width = 0;
	float scene_window_content_area_height = 0;

	float2 scene_window_content_area_pos = float2::zero;
};

#endif //_PANELSCENE_H_