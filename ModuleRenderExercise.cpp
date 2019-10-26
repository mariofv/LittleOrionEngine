#include "Globals.h"
#include "Application.h"
#include "ModuleRenderExercise.h"
#include "ModuleRender.h"
#include "ModuleProgram.h"
#include "ModuleWindow.h"
#include "SDL.h"

ModuleRenderExercise::ModuleRenderExercise()
{
}

// Destructor
ModuleRenderExercise::~ModuleRenderExercise()
{
}

// Called before render is available
bool ModuleRenderExercise::Init()
{
	float buffer_data[] = { 
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		0.0f, 1.0f, 0.0f 
	};

	int windowWidth, windowHeight;
	SDL_GetWindowSize(App->window->window, &windowWidth, &windowHeight);
	float aspect_ratio = (float)windowWidth/ windowHeight;
	
	// CREATES PROJECTION MATRIX
	Frustum frustum;
	frustum.type = FrustumType::PerspectiveFrustum;
	frustum.pos = float3::zero;
	frustum.front = -float3::unitZ;
	frustum.up = float3::unitY;
	frustum.nearPlaneDistance = 0.1f;
	frustum.farPlaneDistance = 100.0f;
	frustum.verticalFov = math::pi / 4.0f;
	frustum.horizontalFov = 2.f * atanf(tanf(frustum.verticalFov * 0.5f) * aspect_ratio);
	proj = frustum.ProjectionMatrix();
	
	// CREATES VIEW MATRIX
	float3 target = -float3::unitZ;
	float3 eye = float3::zero;;
	float3 up = float3::unitY;

	math::float3 f(target - eye); f.Normalize();
	math::float3 s(f.Cross(up)); s.Normalize();
	math::float3 u(s.Cross(f));

	view[0][0] = s.x; view[0][1] = s.y; view[0][2] = s.z;
	view[1][0] = u.x; view[1][1] = u.y; view[1][2] = u.z;
	view[2][0] = -f.x; view[2][1] = -f.y; view[2][2] = -f.z;
	view[0][3] = -s.Dot(eye); view[1][3] = -u.Dot(eye); view[2][3] = f.Dot(eye);
	view[3][0] = 0.0f; view[3][1] = 0.0f; view[3][2] = 0.0f; view[3][3] = 1.0f;

	// CREATES MODEL MATRIX
	model = float4x4::FromTRS(
		float3(0.0f, 0.0f, -4.0f),
		float3x3::RotateY(math::pi/4), 
		float3(1.0f,1.0f, 1.0f)
	);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(buffer_data), buffer_data, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	return true;
}

update_status ModuleRenderExercise::PreUpdate()
{
	glUseProgram(App->program->shader_program);
	glUniformMatrix4fv(
		glGetUniformLocation(App->program->shader_program, "model"),
		1,
		GL_TRUE,
		&model[0][0]
	);
	glUniformMatrix4fv(
		glGetUniformLocation(App->program->shader_program,"view"),
		1,
		GL_TRUE,
		&view[0][0]
	);
	glUniformMatrix4fv(
		glGetUniformLocation(App->program->shader_program,"proj"),
		1,
		GL_TRUE, 
		&proj[0][0]
	);

	return UPDATE_CONTINUE;
}

// Called every draw update
update_status ModuleRenderExercise::Update()
{
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
	glDrawArrays(GL_TRIANGLES, 0, 3); // start at 0 and 3 tris
	glDisableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	return UPDATE_CONTINUE;
}

update_status ModuleRenderExercise::PostUpdate()
{
	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleRenderExercise::CleanUp()
{
	
	return true;
}


