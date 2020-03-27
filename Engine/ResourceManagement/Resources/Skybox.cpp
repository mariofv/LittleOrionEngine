#include "Skybox.h"

#include "Main/Application.h"
#include "Module/ModuleProgram.h"
#include "Module/ModuleResourceManager.h"
#include "Component/ComponentCamera.h"

Skybox::Skybox(Metafile* resource_metafile, const std::array<uint32_t, 6>& textures_id)
{
	GenerateSkyboxCube();
	GenerateTextures(textures_id);
	GenerateSkyboxCubeMap();
}

Skybox::~Skybox()
{
	glDeleteTextures(1, &texture_id);
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
}

void Skybox::Render(const ComponentCamera& camera) const
{
	glDepthFunc(GL_LEQUAL);
	GLuint shader_program = App->program->GetShaderProgramId("Skybox");
	glUseProgram(shader_program);

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
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);
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

void Skybox::GenerateTextures(const std::array<uint32_t, 6>& textures_id)
{
	textures[(size_t)SkyboxFaces::RIGHT] = std::static_pointer_cast<Texture>(App->resources->Load(textures_id[(size_t)SkyboxFaces::RIGHT]));
	textures[(size_t)SkyboxFaces::LEFT] = std::static_pointer_cast<Texture>(App->resources->Load(textures_id[(size_t)SkyboxFaces::LEFT]));

	textures[(size_t)SkyboxFaces::UP] = std::static_pointer_cast<Texture>(App->resources->Load(textures_id[(size_t)SkyboxFaces::UP]));
	textures[(size_t)SkyboxFaces::DOWN] = std::static_pointer_cast<Texture>(App->resources->Load(textures_id[(size_t)SkyboxFaces::DOWN]));

	textures[(size_t)SkyboxFaces::FRONT] = std::static_pointer_cast<Texture>(App->resources->Load(textures_id[(size_t)SkyboxFaces::FRONT]));
	textures[(size_t)SkyboxFaces::BACK] = std::static_pointer_cast<Texture>(App->resources->Load(textures_id[(size_t)SkyboxFaces::BACK]));
}

void Skybox::GenerateSkyboxCubeMap()
{
	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);

	for (size_t i = 0; i < textures.size(); i++)
	{
		std::shared_ptr<Texture> texture = textures[i];
		glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, texture->width, texture->height, 0, texture->image_size, texture->data);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}