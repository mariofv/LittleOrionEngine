#pragma once
#include "Module.h"
#include "Globals.h"

#include <GL/glew.h>

struct SDL_Texture;
struct SDL_Renderer;
struct SDL_Rect;

class ModuleRender : public Module
{
public:
	ModuleRender();
	~ModuleRender();

	bool Init();
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();

	void GenerateFrameTexture(const float width, const float height);
	void GenerateFrameBuffers(const float width, const float height);
	void renderGrid() const;

	void ShowRenderOptions();

private:
	void SetVSync(const bool vsync);
	void SetAlphaTest(const bool gl_alpha_test);
	void SetDepthTest(const bool gl_depth_test);
	void SetScissorTest(const bool gl_scissor_test);
	void SetStencilTest(const bool gl_stencil_test);
	void SetBlending(const bool gl_blend);
	void SetFaceCulling(const bool gl_cull_face);
	void SetDithering(const bool gl_dither);
	void SetMinMaxing(const bool gl_minmax);
	void SetWireframing(const bool gl_wireframe);

public:
	bool bounding_box_visible = false;
	bool model_movement = false;

	GLuint frame_texture;
private:
	GLuint fbo;
	GLuint rbo;

	void* context = nullptr;

	bool vsync = false;
	bool gl_alpha_test = false;
	bool gl_depth_test = false;
	bool gl_scissor_test = false;
	bool gl_stencil_test = false;
	bool gl_blend = false;
	bool gl_cull_face = false;
	bool gl_dither = false;
	bool gl_minmax = false;
	bool gl_wireframe = false;
};
