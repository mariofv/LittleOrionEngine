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

private:

public:
	bool bounding_box_visible = false;
	bool model_movement = false;

	GLuint frame_texture;
private:
	GLuint fbo;
	GLuint rbo;

	void* context;
};
