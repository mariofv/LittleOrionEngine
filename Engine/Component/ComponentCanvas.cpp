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
	//glBindFramebuffer(GL_DRAW_FRAMEBUFFER, App->cameras->scene_camera->fbo);

	//glClear(GL_COLOR_BUFFER_BIT);
	//glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();
	//
	//float canvasW = App->cameras->scene_camera->camera_frustum.orthographicWidth;
	//float canvasH = App->cameras->scene_camera->camera_frustum.orthographicHeight;
	//// args: left, right, bottom, top, near, far
	//glOrtho(0.0f, canvasW, canvasH, 0.0f, 1.0f, -1.0f);
	////Begin drawing quads
	//glBegin(GL_POLYGON);

	//glColor4ub(255, 255, 255, 255);
	//glVertex3f(-0.5, 0.5, -20.f);
	//glVertex3f(canvasW, 0.5, -20.f);
	//glVertex3f(canvasW, canvasH, -20.f);
	//glVertex3f(-0.5, canvasH, -20.f);
	//glEnd();
	////GenerateFBOTexture(App->cameras->scene_camera->camera_frustum.orthographicWidth, App->cameras->scene_camera->camera_frustum.orthographicHeight);
	//
	////dd::xzSquareGrid(-40.0f, 40.0f, 0.0f, 1.0f, math::float3(0.65f, 0.65f, 0.65f));




	//glMatrixMode(GL_MODELVIEW);
	//glDisable(GL_DEPTH_TEST);
	//glRectf(cameraVBO., 5.0, 5.0, -5.0);


	///
	
    //GLfloat vertices[] = { 
    //     Pos      // Tex
    //    0.0f, 1.0f, 0.0f, 1.0f,
    //    1.0f, 0.0f, 1.0f, 0.0f,
    //    0.0f, 0.0f, 0.0f, 0.0f, 

    //    0.0f, 1.0f, 0.0f, 1.0f,
    //    1.0f, 1.0f, 1.0f, 1.0f,
    //    1.0f, 0.0f, 1.0f, 0.0f
    //};
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
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0f, cam.camera_frustum.orthographicWidth, cam.camera_frustum.orthographicHeight, 0.0f, 1.0f, -1.0f);
	glMatrixMode(GL_PROJECTION);
	glDisable(GL_DEPTH_TEST);

	glBindFramebuffer(GL_FRAMEBUFFER, cam.fbo);
	glBindVertexArray(this->vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glUseProgram(0);
}
void ComponentCanvas::Delete()
{
}
void ComponentCanvas::ShowComponentWindow()
{
}

void ComponentCanvas::GenerateFBOTexture(unsigned w, unsigned h)
{
	if (w != fb_width || h != fb_height)
	{
		if (fb_tex != 0)
		{
			glDeleteTextures(1, &fb_tex);
		}
		glGenTextures(1, &fb_tex);
		if (rbo != 0)
		{
			glDeleteRenderbuffers(1, &rbo);
		}
		glGenRenderbuffers(1, &rbo);

		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, App->cameras->scene_camera->camera_frustum.orthographicWidth, App->cameras->scene_camera->camera_frustum.orthographicHeight);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		glBindTexture(GL_TEXTURE_2D, fb_tex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, App->cameras->scene_camera->camera_frustum.orthographicWidth, App->cameras->scene_camera->camera_frustum.orthographicHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);

		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fb_tex, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		fb_width = w;
		fb_height = h;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
