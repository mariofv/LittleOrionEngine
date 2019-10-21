#include "Globals.h"
#include "Application.h"
#include "ModuleRenderExercise.h"
#include "ModuleRender.h"
#include "SDL.h"
#include <GL/glew.h>

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

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(buffer_data), buffer_data, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	return true;
}

update_status ModuleRenderExercise::PreUpdate()
{
	
	

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


