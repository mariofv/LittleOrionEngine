#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleWindow.h"
#include "ModuleProgram.h"
#include "ModuleCamera.h"
#include "ModuleModelLoader.h"
#include "ModuleTime.h"

#include "SDL.h"
#include "MathGeoLib.h"
#include <assimp/scene.h>

ModuleRender::ModuleRender()
{
}

// Destructor
ModuleRender::~ModuleRender()
{
}

// Called before render is available
bool ModuleRender::Init()
{
	LOG("Creating Renderer context");
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

	// Init GLEW library
	GLenum err = glewInit();
	// … check for errors
	if (GLEW_OK != err)
	{
		LOG("Error initializing GLUT");
		return false;
		
	}

	LOG("Using Glew %s", glewGetString(GLEW_VERSION));
	LOG("Vendor: %s", glGetString(GL_VENDOR));
	LOG("Renderer: %s", glGetString(GL_RENDERER));
	LOG("OpenGL version supported %s", glGetString(GL_VERSION));
	LOG("GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	glEnable(GL_DEPTH_TEST);

	glGenFramebuffers(1, &fbo);
	GenerateFrameBuffers(App->window->getWidth(), App->window->getHeight());

	return true;
}

update_status ModuleRender::PreUpdate()
{
	// CLEAR WINDOW COLOR AND DEPTH BUFFER
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	return UPDATE_CONTINUE;
}

// Called every draw update
update_status ModuleRender::Update()
{
	return UPDATE_CONTINUE;
}

update_status ModuleRender::PostUpdate()
{
	SDL_GL_SwapWindow(App->window->window);
	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleRender::CleanUp()
{
	LOG("Destroying renderer");
	glDeleteTextures(1, &frame_texture);
	glDeleteFramebuffers(1, &fbo);
	glDeleteRenderbuffers(1, &rbo);

	return true;
}

void ModuleRender::GenerateFrameTexture(const float width, const float height)
{
	GenerateFrameBuffers(width, height);

	// Bind frame buffer
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// Set viewport
	glViewport(0, 0, width, height);

	// CLEAR FRAME_BUFFER COLOR AND DEPTH BUFFER
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	renderGrid();

	glUseProgram(App->program->texture_program);

	float x_translation = 0.f;

	if (model_movement)
	{
		x_translation = sin(App->time->time * 0.01f)  *5.f;
	}

	// CREATES MODEL MATRIX
	float4x4 model = float4x4::FromTRS(
		float3(x_translation, 0.0f, 0.0f),
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
		&App->cameras->view[0][0]
	);
	glUniformMatrix4fv(
		glGetUniformLocation(App->program->texture_program, "proj"),
		1,
		GL_TRUE,
		&App->cameras->proj[0][0]
	);


	for (unsigned int i = 0; i < App->model_loader->meshes.size(); ++i)
	{
		App->model_loader->meshes[i]->Render(App->program->texture_program);
	}
	glUseProgram(0);

	if (bounding_box_visible)
	{
		App->model_loader->model_bounding_box->Render(App->program->default_program);
	}

	// Unbind frame buffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ModuleRender::GenerateFrameBuffers(const float width, const float height)
{
	if (frame_texture != 0)
	{
		glDeleteTextures(1, &frame_texture);
	}
	glGenTextures(1, &frame_texture);

	if (rbo != 0)
	{
		glDeleteRenderbuffers(1, &rbo);
	}
	glGenRenderbuffers(1, &rbo);
	
	
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	
	glBindTexture(GL_TEXTURE_2D, frame_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frame_texture, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ModuleRender::renderGrid() const
{
	glUseProgram(App->program->primitive_program);

	// CREATES MODEL MATRIX
	float4x4 model = float4x4::FromTRS(
		float3(0.0f, 0.0f, 0.0f),
		float3x3::identity,
		float3(1.0f, 1.0f, 1.0f)
	);
	glUniformMatrix4fv(
		glGetUniformLocation(App->program->primitive_program, "model"),
		1,
		GL_TRUE,
		&model[0][0]
	);
	glUniformMatrix4fv(
		glGetUniformLocation(App->program->primitive_program, "view"),
		1,
		GL_TRUE,
		&App->cameras->view[0][0]
	);
	glUniformMatrix4fv(
		glGetUniformLocation(App->program->primitive_program, "proj"),
		1,
		GL_TRUE,
		&App->cameras->proj[0][0]
	);

	glLineWidth(1.0f);
	float d = 200.0f;
	glBegin(GL_LINES);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	for (float i = -d; i <= d; i += 1.0f)
	{
		glVertex3f(i, 0.0f, -d);
		glVertex3f(i, 0.0f, d);
		glVertex3f(-d, 0.0f, i);
		glVertex3f(d, 0.0f, i);
	}
	glEnd();

	glLineWidth(2.0f);
	glBegin(GL_LINES);
	// red X
	glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(1.0f, 0.0f, 0.0f);
	glVertex3f(1.0f, 0.1f, 0.0f); glVertex3f(1.1f, -0.1f, 0.0f);
	glVertex3f(1.1f, 0.1f, 0.0f); glVertex3f(1.0f, -0.1f, 0.0f);
	// green Y
	glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-0.05f, 1.25f, 0.0f); glVertex3f(0.0f, 1.15f, 0.0f);
	glVertex3f(0.05f, 1.25f, 0.0f); glVertex3f(0.0f, 1.15f, 0.0f);
	glVertex3f(0.0f, 1.15f, 0.0f); glVertex3f(0.0f, 1.05f, 0.0f);
	// blue Z
	glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 0.0f, 1.0f);
	glVertex3f(-0.05f, 0.1f, 1.05f); glVertex3f(0.05f, 0.1f, 1.05f);
	glVertex3f(0.05f, 0.1f, 1.05f); glVertex3f(-0.05f, -0.1f, 1.05f);
	glVertex3f(-0.05f, -0.1f, 1.05f); glVertex3f(0.05f, -0.1f, 1.05f);
	glEnd();
	glLineWidth(1.0f);
}

