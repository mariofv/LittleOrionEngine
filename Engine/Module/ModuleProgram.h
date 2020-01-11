#ifndef _MODULEPROGRAM_H_
#define _MODULEPROGRAM_H_

#include "Module.h"
#include "Globals.h"

#include <GL/glew.h>
#include <MathGeoLib.h>
#include <unordered_map>
class ModuleProgram : public Module
{
public:
	// Holds information of the distibution of data inside the uniform buffer object
	struct UniformBuffer 
	{
		GLuint ubo;

		/*
			Matrices are first in buffer, so no offset is needed for the block.
		*/
		const size_t MATRICES_UNIFORMS_OFFSET = 0;

		/*
			Each matrix ocuppies sizeof(float) * 16 bytes, or in other words, 64 bytes. 
			Also, each matrix column base alignemnt is 16, so each column has to be aligned in mod 16 bytes. 
			Because we are filling memory buffer with only vec4 of floats (16 bytes each) the base alignment is fullfiled.
			
		*/
		const size_t MATRICES_UNIFORMS_SIZE = 3 * sizeof(float4x4); // Size of model, projection and view matrix
		
		/*
			At this point the buffer is filled with Matrices uniform block, wich total size is 192 bytes.
			Following this block we will need the Light uniform block.
			Because we are binding several uniform blocks in the same UBO, we need to align each block with GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT.
			Thus, LIGHT_UNIFORMS_OFFSET will be computed run_time.
		*/
		size_t lights_uniform_offset;

		/*
			In order to compute the total Light uniform block size we have to take in account the alignment of each of its elements. 
			This block has the following elements in this specific order:
				- float
				- float[3]
				- float[3]
			So, the total size will be the following:
				total_size = 4 (float size) + 12 (alignment of float[3]) + 16 (float[3] size) + 16 (float[3] size) = 48 bytes = 12 * sizeof(float) 
		*/
		const size_t LIGHT_UNIFORMS_SIZE = 12 * sizeof(float); // Size of light intensity, color and position


		/*
			Total buffer size depends on the alignment between uniform blocks, so it's size will be computed real time. 
		*/
		size_t uniforms_size;
	};

	ModuleProgram() = default;
	~ModuleProgram() = default;

	bool Init() override;
	bool CleanUp() override;

private:
	bool LoadProgram(std::string name, const char* vertex_shader_file_name, const char* fragment_shader_file_name);
	void LoadPrograms(const char* file_path);
	bool InitVertexShader(GLuint &vertex_shader, const char* vertex_shader_file_name) const;
	bool InitFragmentShader(GLuint &fragment_shader, const char* fragment_shader_file_name) const;
	bool InitProgram(GLuint &shader_program,GLuint vertex_shader,GLuint fragment_shader) const;

	void InitUniformBuffer();
	void BindUniformBlocks(GLuint shader_program) const;

public:
	UniformBuffer uniform_buffer;
	std::unordered_map<std::string, GLuint> loaded_programs;
	std::vector<const char *> names;
};

#endif //_MODULEPROGRAM_H_