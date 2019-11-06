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
	bool loadProgram(GLuint &shader_program, const char* vertex_shader_file_name, const char* fragment_shader_file_name);
	bool initVertexShader(GLuint &vertex_shader, const char* vertex_shader_file_name);
	bool initFragmentShader(GLuint &fragment_shader, const char* fragment_shader_file_name);
	bool initProgram(GLuint &shader_program, const GLuint vertex_shader, const GLuint fragment_shader);
	const char* loadFile(const char *file_name);

public:
	GLuint default_program;
	GLuint texture_program;

};
