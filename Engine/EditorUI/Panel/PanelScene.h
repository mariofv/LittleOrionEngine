#ifndef _PANELSCENE_H_
#define _PANELSCENE_H_

#include "Panel.h"
#include <MathGeoLib.h>

class GameObject;
class PanelConfiguration;
class Viewport;

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
	void RenderDepthMapPreview() const;

	void RenderDebugMetrics() const;

public:
	float scene_window_content_area_width = 0;
	float scene_window_content_area_height = 0;
	float2 scene_window_content_area_pos = float2::zero;

	Viewport* camera_preview_viewport = nullptr;

private:
	bool scene_camera_gizmo_hovered = false;
	bool gizmo_released = false;
	bool control_key_down = false;

};

#endif //_PANELSCENE_H_