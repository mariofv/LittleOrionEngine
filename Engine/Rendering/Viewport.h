#ifndef _VIEWPORT_H_
#define _VIEWPORT_H_

#include <GL/glew.h>

class ComponentCamera;
class FrameBuffer;

class Viewport
{
public:
	Viewport(bool is_scene_viewport);
	~Viewport();

	void SetSize(float width, float height);

	void Render(ComponentCamera* camera);

private:
	void BindCameraMatrices() const;

	void MeshRenderPass() const;
	void EffectsRenderPass() const;
	void UIRenderPass() const;
	void PostProcessPass() const;
	void DebugPass() const;
	void DebugDrawPass() const;
	void EditorDrawPass() const;

public:
	GLuint last_displayed_texture = 0;

	FrameBuffer* render_fbo = 0;
	FrameBuffer* debug_draw_fbo = 0;

private:
	ComponentCamera* camera = nullptr;

	float width = 0;
	float height = 0;

	bool is_scene_viewport = false;
};

#endif //_VIEWPORT_H_

/* 

CURRENT RENDERING PIPELINE
/------------------------/
MESHES -> EFFECTS -> UI -> DEBUG_DRAWS -> EDITOR DRAWS


*/

