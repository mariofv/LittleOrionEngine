#ifndef _MODULEPROGRAM_H_
#define _MODULEPROGRAM_H_

#include "Module.h"
#include "Globals.h"

#include <GL/glew.h>
#include <MathGeoLib.h>

class ModuleProgram : public Module
{
public:
	// Holds information of the distibution of data inside the uniform buffer object
	struct UniformBuffer 
	{
		GLuint ubo;

		const size_t MATRICES_OFFSET = 0;
		const size_t MATRICES_SIZE = 3 * sizeof(float4x4); // Size of model, projection and view matrix
		
		const size_t UNIFORMS_SIZE = MATRICES_SIZE; // Size for uniform buffer.
	};

	ModuleProgram() = default;
	~ModuleProgram() = default;

	bool Init() override;
	bool CleanUp() override;

private:
	bool LoadProgram(GLuint &shader_program, const char* vertex_shader_file_name, const char* fragment_shader_file_name);
	bool InitVertexShader(GLuint &vertex_shader, const char* vertex_shader_file_name) const;
	bool InitFragmentShader(GLuint &fragment_shader, const char* fragment_shader_file_name) const;
	bool InitProgram(GLuint &shader_program,GLuint vertex_shader,GLuint fragment_shader) const;

	void InitUniformBuffer();
	void BindUniformBlocks(GLuint shader_program) const;

public:
	UniformBuffer uniform_buffer;

	GLuint texture_program = 0;
	GLuint skybox_program = 0;
	GLuint linepoint_program = 0;
	GLuint text_program = 0;
	GLuint phong_flat_program = 0;
	GLuint phong_gouraund_program = 0;

	std::vector<GLuint> loaded_programs;
};

#endif //_MODULEPROGRAM_H_