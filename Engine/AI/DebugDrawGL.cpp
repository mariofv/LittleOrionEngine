#include "DebugDrawGL.h"

#include "Main/Application.h"
#include "Module/ModuleProgram.h"
#include "Module/ModuleDebug.h"


void DebugDrawGL::Vertex(const float* pos, unsigned int color)
{
	vertices.push_back(float3(pos));
	colors.push_back(GetColorRGBA(color));
}

float4 DebugDrawGL::GetColorRGBA(unsigned int color) const
{
	unsigned r = color & 0xff;
	unsigned g = (color >> 8) & 0xff;
	unsigned b = (color >> 16) & 0xff;
	unsigned a = (color >> 24) & 0xff;
	float f = 1.0f / 255.0f;
	return float4(r, g, b, a) * f;
}

void DebugDrawGL::DrawMesh(ComponentCamera& camera)
{
	if (vertices.size() == 0)
		return;


	unsigned int shader = App->program->GetShaderProgramId("NavMesh");
	math::float4x4 model = math::float4x4::identity;

	glUseProgram(shader);
	glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_TRUE, &model[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_TRUE, &camera.view[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shader, "proj"), 1, GL_TRUE, &camera.proj[0][0]);

	glEnableVertexAttribArray(0); // attribute 0
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(
		0, // attribute 0
		3, // number of componentes (3 floats)
		GL_FLOAT, // data type
		GL_FALSE, // should be normalized?
		0, // stride
		(void*)0 // array buffer offset
	);

	glEnableVertexAttribArray(1); // attribute 0
	glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
	glVertexAttribPointer(
		1, // attribute 1
		4, // number of componentes (4 floats)
		GL_FLOAT, // data type
		GL_FALSE, // should be normalized?
		0, // stride
		(void*)0 // array buffer offset
	);

	glDrawArrays(GL_TRIANGLES, 0, vertices.size()); // start at 0 and tris count
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glUseProgram(0);
}

void DebugDrawGL::GenerateBuffers()
{
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(float) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &vbo_color);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(float) * colors.size(), &colors[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void DebugDrawGL::CleanUp()
{
	vertices.clear();
	colors.clear();
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &vbo_color);
}
