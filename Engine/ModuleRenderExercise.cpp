#include "Globals.h"
#include "Application.h"
#include "ModuleRenderExercise.h"
#include "ModuleRender.h"
#include "ModuleProgram.h"
#include "ModuleWindow.h"
#include "ModuleTexture.h"
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
	initCamera();

	float vertices[] = {
		// positions          // texture coords
		 0.5f,  0.5f, 0.0f,   1.0f, 1.0f,   // top right
		 0.5f, -0.5f, 0.0f,   1.0f, 0.0f,   // bottom right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f,   // bottom left
		-0.5f,  0.5f, 0.0f,   0.0f, 1.0f    // top left 
	};

	unsigned int indices[] = {  // note that we start from 0!
		0, 1, 3,   // first triangle
		1, 2, 3    // second triangle
	};

	
	// Init VBO, VAO & EBO
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	// 1. bind Vertex Array Object
	glBindVertexArray(vao);

	// 2. copy our vertices array in a vertex buffer for OpenGL to use
	glBindBuffer(GL_ARRAY_BUFFER,vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// 3. copy our index array in a element buffer for OpenGL to use
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// 4. then set the vertex attributes pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0); // position
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float))); // uv
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	


	// LOADS TEXTURE
	App->texture->loadTexture(LENA_TEXTURE_PATH); // TODO: MEMORY LEAK HERE, PENDING TO FREE IMAGE DATA 

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, App->texture->texture_width, App->texture->texture_height, 0, GL_RGB, GL_UNSIGNED_BYTE, App->texture->texture_data);
	glGenerateMipmap(GL_TEXTURE_2D);

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
	glUniform1i(glGetUniformLocation(App->program->shader_program, "u_time"), SDL_GetTicks());

	return UPDATE_CONTINUE;
}

// Called every draw update
update_status ModuleRenderExercise::Update()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(glGetUniformLocation(App->program->shader_program,"texture0"),0);
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	return UPDATE_CONTINUE;
}

update_status ModuleRenderExercise::PostUpdate()
{
	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleRenderExercise::CleanUp()
{
	// TODO: CLEAN UP VAOS AND VBOS
	glBindVertexArray(vao);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	glBindVertexArray(0);

	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);

	return true;
}

void ModuleRenderExercise::initCamera()
{
	int windowWidth, windowHeight;
	SDL_GetWindowSize(App->window->window, &windowWidth, &windowHeight);
	float aspect_ratio = (float)windowWidth / windowHeight;

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
		float3x3::RotateY(0),
		float3(1.0f, 1.0f, 1.0f)
	);

}


