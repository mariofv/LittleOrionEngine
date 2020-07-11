#include "Grid.h"

#include "Component/ComponentCamera.h"
#include "Main/Application.h"
#include "Module/ModuleCamera.h"
#include "Module/ModuleProgram.h"

#include <Brofiler/Brofiler.h>

Grid::Grid()
{
	float vertices[] = {
		// positions          
		 1.0f, 0.0f, 1.0f,    // top right
		 1.0f, 0.0f, -1.0f,    // bottom right
		-1.0f,	0.0f, -1.0f,     // bottom left
		-1.0f, 0.0f, 1.0f    // top left 
	};

	unsigned int indices[] = {  // note that we start from 0!
		0, 1, 3,  // first Triangle
		1, 2, 3   // second Triangle
	};

	glGenVertexArrays(1, &grid_vao);
	glGenBuffers(1, &grid_vbo);
	glGenBuffers(1, &grid_ebo);

	glBindVertexArray(grid_vao);

	glBindBuffer(GL_ARRAY_BUFFER, grid_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, grid_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}


Grid::~Grid()
{
	glDeleteVertexArrays(1, &grid_vao);
	glDeleteBuffers(1, &grid_vbo);
	glDeleteBuffers(1, &grid_ebo);
}

void Grid::Render() const
{
	BROFILER_CATEGORY("Render Grid", Profiler::Color::BlueViolet);
	glEnable(GL_BLEND);
	GLuint grid_program = App->program->UseProgram("Grid");

	float4x4 model_matrix = float4x4::FromTRS
	(
		float3::zero,
		Quat::identity,
		float3(grid_size)
	);

	glBindBuffer(GL_UNIFORM_BUFFER, App->program->uniform_buffer.ubo);
	glBufferSubData(GL_UNIFORM_BUFFER, App->program->uniform_buffer.MATRICES_UNIFORMS_OFFSET, sizeof(float4x4), model_matrix.Transposed().ptr());
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glUniform1f(glGetUniformLocation(grid_program, "grid.grid_size"), grid_size);
	glUniform1f(glGetUniformLocation(grid_program, "grid.cell_size"), cell_size);
	glUniform4fv(glGetUniformLocation(grid_program, "grid.thin_color"), 1, thin_lines_color.ptr());
	glUniform4fv(glGetUniformLocation(grid_program, "grid.thick_color"), 1, thick_lines_color.ptr());

	glBindVertexArray(grid_vao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0); // no need to unbind it every time 
	glDisable(GL_BLEND);
	glUseProgram(0);
}

void Grid::ScaleOnDistance(float distance)
{
	grid_size = distance * DISTANCE_FACTOR;
}
