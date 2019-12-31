#ifndef _MODULEPROGRAM_H_
#define _MODULEPROGRAM_H_

#include "Module.h"
#include "Globals.h"

#include <GL/glew.h>

class ModuleProgram : public Module
{
public:
	ModuleProgram() = default;
	~ModuleProgram() = default;

	bool Init() override;
	bool CleanUp() override;

private:
	bool LoadProgram(GLuint &shader_program, const char* vertex_shader_file_name, const char* fragment_shader_file_name);
	bool InitVertexShader(GLuint &vertex_shader, const char* vertex_shader_file_name) const;
	bool InitFragmentShader(GLuint &fragment_shader, const char* fragment_shader_file_name) const;
	bool InitProgram(GLuint &shader_program,GLuint vertex_shader,GLuint fragment_shader) const;

public:
	GLuint texture_program = 0;
	GLuint skybox_program = 0;
	GLuint linepoint_program = 0;
	GLuint text_program = 0;
	GLuint phong_flat_program = 0;
	GLuint phong_gouraund_program = 0;

	std::vector<GLuint> loaded_programs;
};

#endif //_MODULEPROGRAM_H_