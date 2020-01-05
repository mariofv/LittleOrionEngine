#include "Application.h"
#include "ModuleProgram.h"
#include "ModuleFileSystem.h"

#include "MathGeoLib.h"

// Called before render is available
bool ModuleProgram::Init()
{
	APP_LOG_SECTION("************ Module Program Init ************");

	InitUniformBuffer();

	if (!LoadProgram(texture_program, DEFAULT_VERTEX_SHADER_PATH, TEXTURE_FRAGMENT_SHADER_PATH))
	{
		return false;
	}
	if (!LoadProgram(skybox_program, SKYBOX_VERTEX_SHADER_PATH, SKYBOX_FRAGMENT_SHADER_PATH))
	{
		return false;
	}

	if (!LoadProgram(linepoint_program, LINEPOINT_VERTEX_SHADER_PATH, LINEPOINT_FRAGMENT_SHADER_PATH))
	{
		return false;
	}

	if (!LoadProgram(text_program, TEXT_VERTEX_SHADER_PATH, TEXT_FRAGMENT_SHADER_PATH))
	{
		return false;
	}

	if (!LoadProgram(phong_flat_program, FLAT_VERTEX_SHADER_PATH, FLAT_FRAGMENT_SHADER_PATH))
	{
		return false;
	}

	if (!LoadProgram(phong_gouraund_program, GOURAUND_VERTEX_SHADER_PATH, GOURAUND_FRAGMENT_SHADER_PATH))
	{
		return false;
	}

	if (!LoadProgram(phong_phong_program, PHONG_VERTEX_SHADER_PATH, PHONG_FRAGMENT_SHADER_PATH))
	{
		return false;
	}

	return true;
}

// Called before quitting
bool ModuleProgram::CleanUp()
{
	for ( auto & program : loaded_programs)
	{
		glDeleteProgram(program);
	}

	glDeleteBuffers(1, &uniform_buffer.ubo);

	return true;
}

void ModuleProgram::InitUniformBuffer()
{
	int uniform_buffer_offset_alignment;
	glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &uniform_buffer_offset_alignment);

	uniform_buffer.lights_uniform_offset = uniform_buffer_offset_alignment * CeilInt(((float)uniform_buffer.MATRICES_UNIFORMS_SIZE) / uniform_buffer_offset_alignment);
	
	uniform_buffer.uniforms_size = uniform_buffer.lights_uniform_offset + uniform_buffer.LIGHT_UNIFORMS_SIZE;

	glGenBuffers(1, &uniform_buffer.ubo);

	glBindBuffer(GL_UNIFORM_BUFFER, uniform_buffer.ubo);
	glBufferData(GL_UNIFORM_BUFFER, uniform_buffer.uniforms_size, NULL, GL_STATIC_DRAW); // Allocate space in uniform buffer
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	// Bind buffer ranges with binding points. NOTE: ORDER MATTERS!
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, uniform_buffer.ubo, uniform_buffer.MATRICES_UNIFORMS_OFFSET, uniform_buffer.MATRICES_UNIFORMS_SIZE); // Sets binding point 0 for model, projection and view matrix
	glBindBufferRange(GL_UNIFORM_BUFFER, 1, uniform_buffer.ubo, uniform_buffer.lights_uniform_offset, uniform_buffer.LIGHT_UNIFORMS_SIZE); // Sets binding point 1 for light intensity, color and position
}

bool ModuleProgram::LoadProgram(GLuint &shader_program, const char* vertex_shader_file_name, const char* fragment_shader_file_name)
{
	APP_LOG_INIT("Loading shader program. With paths: %s,%s", vertex_shader_file_name, fragment_shader_file_name);
	GLuint vertex_shader;
	GLuint fragment_shader;
	if (!InitVertexShader(vertex_shader, vertex_shader_file_name))
	{
		return false;
	}

	if (!InitFragmentShader(fragment_shader, fragment_shader_file_name))
	{
		return false;
	}

	if (!InitProgram(shader_program, vertex_shader, fragment_shader))
	{
		return false;
	}
	loaded_programs.push_back(shader_program);
	APP_LOG_SUCCESS("Shader program with paths: %s,%s loaded correctly.", vertex_shader_file_name, fragment_shader_file_name);
	return true;
}

bool ModuleProgram::InitVertexShader(GLuint &vertex_shader, const char* vertex_shader_file_name) const
{
	APP_LOG_INFO("Loading vertex shader");
	size_t size;
	const char *vertex_shader_loaded_file = App->filesystem->Load(vertex_shader_file_name, size);
	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	if (vertex_shader == 0) {
		APP_LOG_ERROR("Error creating vertex shader %s", vertex_shader_file_name);
		return false;
	}
	glShaderSource(vertex_shader, 1, &vertex_shader_loaded_file, NULL);
	delete vertex_shader_loaded_file;

	APP_LOG_INFO("Compiling vertex shader");
	glCompileShader(vertex_shader);
	int compilation_status;
	char info_log[512];
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &compilation_status);
	if (!compilation_status)
	{
		glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
		APP_LOG_ERROR("Error compiling vertex shader %s", vertex_shader_file_name);
		APP_LOG_ERROR(info_log);
		return false;
	}

	return true;
}

bool ModuleProgram::InitFragmentShader(GLuint &fragment_shader, const char* fragment_shader_file_name) const
{
	APP_LOG_INFO("Loading fragment shader");
	size_t size;
	const char *fragment_shader_loaded_file = App->filesystem->Load(fragment_shader_file_name, size);
	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	if (fragment_shader == 0) {
		OPENGL_LOG_ERROR("Error creating fragment shader %s", fragment_shader_file_name);
		return false;
	}
	glShaderSource(fragment_shader, 1, &fragment_shader_loaded_file, NULL);
	delete fragment_shader_loaded_file;

	APP_LOG_INFO("Compiling fragment shader");
	glCompileShader(fragment_shader);
	int compilation_status;
	char info_log[512];
	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &compilation_status);
	if (!compilation_status)
	{
		glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);
		APP_LOG_ERROR("Error compiling fragment shader %s", fragment_shader_file_name);
		APP_LOG_ERROR(info_log);
		return false;
	}

	return true;
}

bool ModuleProgram::InitProgram(GLuint &shader_program, const GLuint vertex_shader, const GLuint fragment_shader) const
{
	APP_LOG_INFO("Creating shader program");
	shader_program = glCreateProgram();
	if (shader_program == 0) {
		APP_LOG_ERROR("Error creating shader program");
		return false;
	}

	APP_LOG_INFO("Attaching vertex shader to shader program");
	glAttachShader(shader_program, vertex_shader);

	APP_LOG_INFO("Attaching fragment shader to shader program");
	glAttachShader(shader_program, fragment_shader);

	APP_LOG_INFO("Linking shader program");
	glLinkProgram(shader_program);

	APP_LOG_INFO("Deleting vertex shader");
	glDeleteShader(vertex_shader);

	APP_LOG_INFO("Deleting fragment shader");
	glDeleteShader(fragment_shader);

	int compilation_status;
	char info_log[512];
	glGetProgramiv(shader_program, GL_LINK_STATUS, &compilation_status);
	if (!compilation_status) {
		glGetProgramInfoLog(shader_program, 512, NULL, info_log);
		APP_LOG_ERROR("Error linking shader program");
		APP_LOG_ERROR(info_log);
		return false;
	}

	BindUniformBlocks(shader_program);

	return true;
}

void ModuleProgram::BindUniformBlocks(GLuint shader_program) const
{
	glUniformBlockBinding(shader_program, glGetUniformBlockIndex(shader_program, "Matrices"), 0);
	glUniformBlockBinding(shader_program, glGetUniformBlockIndex(shader_program, "Light"), 1);
}