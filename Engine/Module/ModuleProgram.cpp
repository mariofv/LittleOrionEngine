#include "Globals.h"
#include "Application.h"
#include "ModuleProgram.h"

#include <SDL/SDL.h>

// Called before render is available
bool ModuleProgram::Init()
{
	APP_LOG_SECTION("************ Module Program Init ************");

	APP_LOG_INIT("Loading default shader program.");
	if (LoadProgram(default_program, DEFAULT_VERTEX_SHADER_PATH, DEFAULT_FRAGMENT_SHADER_PATH))
	{
		APP_LOG_SUCCESS("Default shader program loaded correctly.");
	}
	else 
	{
		APP_LOG_ERROR("Default shader program loaded incorretly.");
		return false;
	}

	APP_LOG_INIT("Loading texture shader program.");
	if (LoadProgram(texture_program, DEFAULT_VERTEX_SHADER_PATH, TEXTURE_FRAGMENT_SHADER_PATH))
	{
		APP_LOG_SUCCESS("Texture shader program loaded correctly.");
	}
	else
	{
		APP_LOG_ERROR("Texture shader program loaded incorretly.");
		return false;
	}

	APP_LOG_INIT("Loading skybox shader program.");
	if (LoadProgram(skybox_program, SKYBOX_VERTEX_SHADER_PATH, SKYBOX_FRAGMENT_SHADER_PATH))
	{
		APP_LOG_SUCCESS("Skybox shader program loaded correctly.");
	}
	else
	{
		APP_LOG_ERROR("Skybox shader program loaded incorretly.");
		return false;
	}

	APP_LOG_INIT("Loading linepoint shader program.");
	if (LoadProgram(linepoint_program, LINEPOINT_VERTEX_SHADER_PATH, LINEPOINT_FRAGMENT_SHADER_PATH))
	{
		APP_LOG_SUCCESS("Linepoint shader program loaded correctly.");
	}
	else
	{
		APP_LOG_ERROR("Linepoint shader program loaded incorretly.");
		return false;
	}

	APP_LOG_INIT("Loading text shader program.");
	if (LoadProgram(text_program, TEXT_VERTEX_SHADER_PATH, TEXT_FRAGMENT_SHADER_PATH))
	{
		APP_LOG_SUCCESS("Text shader program loaded correctly.");
	}
	else
	{
		APP_LOG_ERROR("Text shader program loaded incorretly.");
		return false;
	}

	return true;
}

// Called before quitting
bool ModuleProgram::CleanUp()
{
	glDeleteProgram(default_program);
	glDeleteProgram(texture_program);
	glDeleteProgram(skybox_program);
	glDeleteProgram(linepoint_program);
	glDeleteProgram(texture_program);
	return true;
}

bool ModuleProgram::LoadProgram(GLuint &shader_program, const char* vertex_shader_file_name, const char* fragment_shader_file_name) const
{
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

	return true;
}

bool ModuleProgram::InitVertexShader(GLuint &vertex_shader, const char* vertex_shader_file_name) const
{
	APP_LOG_INFO("Loading vertex shader");
	const char *vertex_shader_loaded_file = LoadFile(vertex_shader_file_name);
	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	if (vertex_shader == 0) {
		APP_LOG_ERROR("Error creating vertex shader");
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
		APP_LOG_ERROR("Error compiling vertex shader");
		APP_LOG_ERROR(info_log);
		return false;
	}

	return true;
}

bool ModuleProgram::InitFragmentShader(GLuint &fragment_shader, const char* fragment_shader_file_name) const
{
	APP_LOG_INFO("Loading fragment shader");
	const char *fragment_shader_loaded_file = LoadFile(fragment_shader_file_name);
	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	if (fragment_shader == 0) {
		OPENGL_LOG_ERROR("Error creating fragment shader");
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
		APP_LOG_ERROR("Error compiling fragment shader");
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

	return true;
}

const char* ModuleProgram::LoadFile(const char* file_name) const
{
	char* data = nullptr;
	FILE* file = nullptr;
	fopen_s(&file, file_name, "rb");
	if (file)
	{
		fseek(file, 0, SEEK_END);
		int size = ftell(file);
		rewind(file);
		data = (char*)malloc(size + 1);
		fread(data, 1, size, file);
		data[size] = 0;
		fclose(file);
	}

	return data;
}