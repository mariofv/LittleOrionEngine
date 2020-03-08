#include "ComponentCanvas.h"
#include "./Module/ModuleCamera.h"
#include "./Module/ModuleDebugDraw.h"
#include "./Module/ModuleProgram.h"
#include "Main/Application.h"
#include "UI/DebugDraw.h" 

ComponentCanvas::ComponentCanvas() : Component(nullptr, ComponentType::CANVAS)
{
	InitCanvas();
}

ComponentCanvas::ComponentCanvas(GameObject * owner) : Component(owner, ComponentType::CANVAS)
{
	InitCanvas();
}

void ComponentCanvas::InitCanvas()
{
    glGenVertexArrays(1, &this->vao);
	glBindVertexArray(this->vao);

	glGenBuffers(1, &vbo);

	GLfloat vertices[] = {
	 -1.0f,  1.0f, 1.0f, 0.0f, 0.0f, // Top-left
	 1.0f,  1.0f, 0.0f, 1.0f, 0.0f, // Top-right
	 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, // Bottom-right
	-1.0f, -1.0f, 1.0f, 1.0f, 1.0f  // Bottom-left
	};

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &ebo);

	GLuint elements[] = {
	0, 1, 2,
	2, 3, 0
	};

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);


    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


ComponentCanvas::~ComponentCanvas()
{
	//glDeleteFramebuffers(1, &fbo);
}

void ComponentCanvas::Save(Config& config) const
{
}

void ComponentCanvas::Load(const Config& config)
{

}
void ComponentCanvas::Render(const ComponentCamera& cam) const
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0f, cam.camera_frustum.orthographicWidth, cam.camera_frustum.orthographicHeight, 0.0f, 1.0f, -1.0f);
	glDisable(GL_DEPTH_TEST);

	glBindFramebuffer(GL_FRAMEBUFFER, cam.fbo);
	glBindVertexArray(this->vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glUseProgram(0);

	glEnable(GL_DEPTH_TEST);
}
void ComponentCanvas::Delete()
{
}
void ComponentCanvas::ShowComponentWindow()
{
}
