#ifndef _VIEWPORT_H_
#define _VIEWPORT_H_

#include <GL/glew.h>

class ComponentCamera;
class FrameBuffer;
class MultiSampledFrameBuffer;

class Viewport
{
public:
	Viewport(bool is_scene_viewport);
	~Viewport();

	void Render(ComponentCamera* camera);

	void SetSize(float width, float height);
	void SetAntialiasing(bool antialiasing);

private:
	void BindCameraMatrices() const;

	void MeshRenderPass() const;
	void EffectsRenderPass() const;
	void UIRenderPass() const;
	void PostProcessPass() const;
	void DebugPass() const;
	void DebugDrawPass() const;
	void EditorDrawPass() const;

	void SelectLastDisplayedTexture();

public:
	GLuint last_displayed_texture = 0;

	FrameBuffer* main_fbo = nullptr;
	FrameBuffer* regular_fbo = nullptr;
	MultiSampledFrameBuffer* multisampled_fbo = nullptr;

	bool effects_pass = true;
	bool debug_pass = true;
	bool debug_draw_pass = true;


private:
	ComponentCamera* camera = nullptr;

	float width = 0;
	float height = 0;

	bool is_scene_viewport = false;
	bool antialiasing = true;
};

#endif //_VIEWPORT_H_

/* 

CURRENT RENDERING PIPELINE
/------------------------/
MESHES -> EFFECTS -> UI -> POST_PROCESS -> DEBUG_DRAWS -> EDITOR DRAWS

*/

