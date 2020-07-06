#include "Skybox.h"

#include "Component/ComponentCamera.h"
#include "Helper/Config.h"
#include "Main/Application.h"
#include "Module/ModuleProgram.h"
#include "Module/ModuleResourceManager.h"
#include "ResourceManagement/Manager/SkyboxManager.h"

Skybox::Skybox() : Resource()
{
	GenerateSkyboxCube();
	GenerateSkyboxCubeMap();
}

Skybox::Skybox(uint32_t uuid) : Resource(uuid)
{
	GenerateSkyboxCube();
	GenerateSkyboxCubeMap();
}

Skybox::~Skybox()
{
	glDeleteTextures(1, &cubemap);
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
}

void Skybox::Save(Config& config) const
{
	config.AddUInt(textures_id[(size_t)SkyboxFace::RIGHT], "Right");
	config.AddUInt(textures_id[(size_t)SkyboxFace::LEFT], "Left");

	config.AddUInt(textures_id[(size_t)SkyboxFace::UP], "Up");
	config.AddUInt(textures_id[(size_t)SkyboxFace::DOWN], "Down");

	config.AddUInt(textures_id[(size_t)SkyboxFace::FRONT], "Front");
	config.AddUInt(textures_id[(size_t)SkyboxFace::BACK], "Back");
}

void Skybox::Load(const Config& config)
{
	textures_id[(size_t)SkyboxFace::RIGHT] = config.GetUInt32("Right", 0);
	textures_id[(size_t)SkyboxFace::LEFT] = config.GetUInt32("Left", 0);

	textures_id[(size_t)SkyboxFace::UP] = config.GetUInt32("Up", 0);
	textures_id[(size_t)SkyboxFace::DOWN] = config.GetUInt32("Down", 0);

	textures_id[(size_t)SkyboxFace::FRONT] = config.GetUInt32("Front", 0);
	textures_id[(size_t)SkyboxFace::BACK] = config.GetUInt32("Back", 0);

	GenerateSkyboxCubeMap();
}

void Skybox::Render(const ComponentCamera& camera) const
{
	glDepthFunc(GL_LEQUAL);
	GLuint shader_program = App->program->UseProgram("Skybox");

	float4x4 view_matrix = camera.GetViewMatrix();
	view_matrix.SetRow(3, float4::zero);
	view_matrix.SetCol(3, float4::zero);

	glUniformMatrix4fv(
		glGetUniformLocation(shader_program, "view"),
		1,
		GL_TRUE,
		view_matrix.ptr()
	);
	glUniformMatrix4fv(
		glGetUniformLocation(shader_program, "proj"),
		1,
		GL_TRUE,
		camera.GetProjectionMatrix().ptr()
	);


	//Draw skybox
	glBindVertexArray(vao);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);

	glDepthFunc(GL_LESS);
	glUseProgram(0);
}

void Skybox::GenerateSkyboxCube()
{
	float skybox_vertices[] = {
		// positions          
		-5.0f,  5.0f, -5.0f,
		-5.0f, -5.0f, -5.0f,
		 5.0f, -5.0f, -5.0f,
		 5.0f, -5.0f, -5.0f,
		 5.0f,  5.0f, -5.0f,
		-5.0f,  5.0f, -5.0f,

		-5.0f, -5.0f,  5.0f,
		-5.0f, -5.0f, -5.0f,
		-5.0f,  5.0f, -5.0f,
		-5.0f,  5.0f, -5.0f,
		-5.0f,  5.0f,  5.0f,
		-5.0f, -5.0f,  5.0f,

		 5.0f, -5.0f, -5.0f,
		 5.0f, -5.0f,  5.0f,
		 5.0f,  5.0f,  5.0f,
		 5.0f,  5.0f,  5.0f,
		 5.0f,  5.0f, -5.0f,
		 5.0f, -5.0f, -5.0f,

		-5.0f, -5.0f,  5.0f,
		-5.0f,  5.0f,  5.0f,
		 5.0f,  5.0f,  5.0f,
		 5.0f,  5.0f,  5.0f,
		 5.0f, -5.0f,  5.0f,
		-5.0f, -5.0f,  5.0f,

		-5.0f,  5.0f, -5.0f,
		 5.0f,  5.0f, -5.0f,
		 5.0f,  5.0f,  5.0f,
		 5.0f,  5.0f,  5.0f,
		-5.0f,  5.0f,  5.0f,
		-5.0f,  5.0f, -5.0f,

		-5.0f, -5.0f, -5.0f,
		-5.0f, -5.0f,  5.0f,
		 5.0f, -5.0f, -5.0f,
		 5.0f, -5.0f, -5.0f,
		-5.0f, -5.0f,  5.0f,
		 5.0f, -5.0f,  5.0f
	};

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skybox_vertices), &skybox_vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	glBindVertexArray(0);
}


void Skybox::GenerateSkyboxCubeMap()
{
	glGenTextures(1, &cubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);

	for (size_t i = 0; i <= (size_t)SkyboxFace::BACK; i++)
	{
		uint32_t texture_id = textures_id[i];
		if (texture_id != 0)
		{
			textures[i] = App->resources->Load<Texture>(texture_id);
			std::shared_ptr<Texture> texture = textures[i];
			glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, texture->width, texture->height, 0, texture->data.size(), texture->data.data());
		}
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}