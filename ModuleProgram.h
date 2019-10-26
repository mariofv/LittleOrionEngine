#pragma once
#include "Module.h"
#include "Globals.h"
#include <GL/glew.h>

struct SDL_Texture;
struct SDL_Renderer;
struct SDL_Rect;

class ModuleProgram : public Module
{
public:
	ModuleProgram();
	~ModuleProgram();

	bool Init();
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();

private:
	bool initVertexShader();
	bool initFragmentShader();
	bool initProgram();
	const char* loadFile(const char *file_name);

public:
	GLuint shader_program;

private:
	GLuint vertex_shader;
	GLuint fragment_shader;
};
