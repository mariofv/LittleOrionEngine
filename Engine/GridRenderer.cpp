#include "GridRenderer.h"
#include "Globals.h"
#include "Application.h"
#include "Module/ModuleTexture.h"
#include "Module/ModuleProgram.h"
#include "Importer/MaterialImporter.h"


GridRenderer::GridRenderer()
{
	InitGridPlane();
	InitGridTexture();
}

GridRenderer::~GridRenderer()
{
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);
	glDeleteVertexArrays(1, &vao);
	App->material_importer->RemoveTextureFromCacheIfNeeded(texture);
}

void GridRenderer::InitGridPlane()
{
	float vertices_data[20] =
	{
		-size / 2, 0.0f, size / 2,
		size / 2, 0.0f, size / 2,
		size / 2, 0.0f, -size / 2,
		-size / 2, 0.0f, -size / 2,

		0.0, 0.0,
		100.0, 0.0,
		100.0, 100.0,
		0.0, 100.0
	};

	unsigned int indices[6] =
	{
		0,1,2,
		0,2,3
	};

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glBufferData(GL_ARRAY_BUFFER, 20 * sizeof(float), &vertices_data[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	// VERTEX POSITION
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	// VERTEX POSITION
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)(12 * sizeof(float)));

	glBindVertexArray(0);
}

void GridRenderer::InitGridTexture()
{
	texture = App->texture->LoadTexture(GRID_TEXTURE_PATH);

	texture->SetWrapS(GL_REPEAT);
	texture->SetWrapT(GL_REPEAT);
	texture->SetMinFilter(GL_LINEAR_MIPMAP_LINEAR);
	texture->SetMagFilter(GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, texture->opengl_texture);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16);
	glBindTexture(GL_TEXTURE_2D, 0);

	texture->GenerateMipMap();
}

void GridRenderer::Render(const ComponentCamera &camera) const
{
	

	glUseProgram(App->program->texture_program);

	// CREATES MODEL MATRIX
	float4x4 model = float4x4::FromTRS(
		float3(0.0f, 0.0f, 0.0f),
		float3x3::identity,
		float3(1.0f, 1.0f, 1.0f)
	);
	glUniformMatrix4fv(
		glGetUniformLocation(App->program->texture_program, "model"),
		1,
		GL_TRUE,
		&model[0][0]
	);
	glUniformMatrix4fv(
		glGetUniformLocation(App->program->texture_program, "view"),
		1,
		GL_TRUE,
		&camera.GetViewMatrix()[0][0]
	);
	glUniformMatrix4fv(
		glGetUniformLocation(App->program->texture_program, "proj"),
		1,
		GL_TRUE,
		&camera.GetProjectionMatrix()[0][0]
	);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture->opengl_texture);
	glUniform1i(glGetUniformLocation(App->program->texture_program, "texture0"), 0);

	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glUseProgram(0);

}
