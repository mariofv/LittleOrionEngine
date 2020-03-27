#include "Skybox.h"

#include "Helper/Config.h"
#include "Main/Application.h"
#include "Module/ModuleProgram.h"
#include "Module/ModuleResourceManager.h"
#include "Component/ComponentCamera.h"

Skybox::Skybox()
{
	GenerateSkyboxCube();
	GenerateSkyboxCubeMap();
}

Skybox::Skybox(Metafile* resource_metafile)
{
	GenerateSkyboxCube();
	GenerateSkyboxCubeMap();
}

Skybox::Skybox(Metafile* resource_metafile, const std::array<uint32_t, 6>& textures_id)
{
	GenerateSkyboxCube();
	GenerateTextures();
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
	config.AddUInt(textures_id[(size_t)SkyboxFaces::RIGHT], "Right");
	config.AddUInt(textures_id[(size_t)SkyboxFaces::LEFT], "Left");

	config.AddUInt(textures_id[(size_t)SkyboxFaces::UP], "Up");
	config.AddUInt(textures_id[(size_t)SkyboxFaces::DOWN], "Down");

	config.AddUInt(textures_id[(size_t)SkyboxFaces::FRONT], "Front");
	config.AddUInt(textures_id[(size_t)SkyboxFaces::BACK], "Back");
}

void Skybox::Load(const Config& config)
{
	textures_id[(size_t)SkyboxFaces::RIGHT] = config.GetUInt("Right", 0);
	textures_id[(size_t)SkyboxFaces::LEFT] = config.GetUInt("Left", 0);

	textures_id[(size_t)SkyboxFaces::UP] = config.GetUInt("Up", 0);
	textures_id[(size_t)SkyboxFaces::DOWN] = config.GetUInt("Down", 0);

	textures_id[(size_t)SkyboxFaces::FRONT] = config.GetUInt("Front", 0);
	textures_id[(size_t)SkyboxFaces::BACK] = config.GetUInt("Back", 0);

	GenerateTextures();
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

void Skybox::GenerateTextures()
{
	GenerateTexture(SkyboxFaces::RIGHT);
	GenerateTexture(SkyboxFaces::LEFT);

	GenerateTexture(SkyboxFaces::UP);
	GenerateTexture(SkyboxFaces::DOWN);

	GenerateTexture(SkyboxFaces::FRONT);
	GenerateTexture(SkyboxFaces::BACK);
}

void Skybox::GenerateTexture(SkyboxFaces face)
{
	uint32_t texture_id = textures_id[(size_t)face];
	if (texture_id != 0)
	{
		textures[(size_t)face] = std::static_pointer_cast<Texture>(App->resources->Load(texture_id));
	}
}

void Skybox::GenerateSkyboxCubeMap()
{
	glGenTextures(1, &cubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);

	for (size_t i = 0; i < textures.size(); i++)
	{
		if (textures_id[i] != 0)
		{
			std::shared_ptr<Texture> texture = textures[i];
			glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, texture->width, texture->height, 0, texture->image_size, texture->data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}