#include "GeometryRenderer.h"
#include "Application.h"
#include "Globals.h"
#include "Module/ModuleProgram.h"
#include "Component/ComponentCamera.h"


GeometryRenderer::GeometryRenderer()
{
	InitHexahedron();
}

GeometryRenderer::~GeometryRenderer()
{
	delete hexahedron;
}

void GeometryRenderer::InitHexahedron()
{
	hexahedron = new Geometry;

	hexahedron->num_indices = 12;
	unsigned int indices[] = {
		 0, 4, 6, 2,
		 1, 5, 7, 3,
		 0, 1, 4, 5,
		 6, 7, 2, 3
	};

	glBindVertexArray(hexahedron->vao);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, hexahedron->ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

void GeometryRenderer::RenderGeometry(const ComponentCamera &camera, const GeometryRenderer::Geometry &geometry) const
{
	glUseProgram(App->program->default_program);

	float4x4 model_matrix = float4x4::identity;

	glUniformMatrix4fv(
		glGetUniformLocation(App->program->default_program, "model"),
		1,
		GL_TRUE,
		&model_matrix[0][0]
	);
	glUniformMatrix4fv(
		glGetUniformLocation(App->program->default_program, "view"),
		1,
		GL_TRUE,
		&camera.GetViewMatrix()[0][0]
	);
	glUniformMatrix4fv(
		glGetUniformLocation(App->program->default_program, "proj"),
		1,
		GL_TRUE,
		&camera.GetProjectionMatrix()[0][0]
	);

	glBindVertexArray(geometry.vao);
	glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, 0);
	glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, (GLvoid*)(4 * sizeof(unsigned int)));
	glDrawElements(GL_LINES, 8, GL_UNSIGNED_INT, (GLvoid*)(8 * sizeof(unsigned int)));
	glBindVertexArray(0);

	glUseProgram(0);
}

void GeometryRenderer::RenderHexahedron(const ComponentCamera &camera, const std::vector<float> &vertices)
{
	glBindVertexArray(hexahedron->vao);

	glBindBuffer(GL_ARRAY_BUFFER, hexahedron->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 24, &vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,  // attribute
		3,                  // number of elements per vertex, here (x,y,z)
		GL_FLOAT,           // the type of each element
		GL_FALSE,           // take our values as-is
		0,                  // no extra data between each position
		0                   // offset of first element
	);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	RenderGeometry(camera, *hexahedron);
}
