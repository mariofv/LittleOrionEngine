#include "DebugDrawGL.h"

#include "Main/Application.h"
#include "Module/ModuleProgram.h"

void DebugDrawGL::DepthMask(bool state)
{
	glDepthMask(state ? GL_TRUE : GL_FALSE);
}

void DebugDrawGL::Texture(bool state)
{
	if (state)
	{
		glEnable(GL_TEXTURE_2D);
		g_tex.Bind();
	}
	else
	{
		glDisable(GL_TEXTURE_2D);
	}
}

void DebugDrawGL::Begin(DuDebugDrawPrimitives prim, float size)
{
	switch (prim)
	{
	case DU_DRAW_POINTS:
		glPointSize(size);
		glBegin(GL_POINTS);
		break;
	case DU_DRAW_LINES:
		glLineWidth(size);
		glBegin(GL_LINES);
		break;
	case DU_DRAW_TRIS:
		glBegin(GL_TRIANGLES);
		break;
	case DU_DRAW_QUADS:
		glBegin(GL_QUADS);
		break;
	};
}

void DebugDrawGL::Vertex(const float* pos, unsigned int color)
{
	//glColor3f(1.f, 0.f, 0.f);
	//glVertex3fv(pos);
	vertices.push_back(math::float3(pos));
}

void DebugDrawGL::Vertex(const float x, const float y, const float z, unsigned int color)
{
	glColor4ubv((GLubyte*)&color);
	glVertex3f(x, y, z);
}

void DebugDrawGL::Vertex(const float* pos, unsigned int color, const float* uv)
{
	glColor4ubv((GLubyte*)&color);
	glTexCoord2fv(uv);
	glVertex3fv(pos);
}

void DebugDrawGL::Vertex(const float x, const float y, const float z, unsigned int color, const float u, const float v)
{
	glColor4ubv((GLubyte*)&color);
	glTexCoord2f(u, v);
	glVertex3f(x, y, z);
}

void DebugDrawGL::End()
{
	glEnd();
	glLineWidth(3.0f);
	glPointSize(4.0f);
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


	glDrawArrays(GL_TRIANGLES, 0, vertices.size()); // start at 0 and tris count
	glDisableVertexAttribArray(0);
	glUseProgram(0);
}

void DebugDrawGL::GenerateBuffers()
{
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(float) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
