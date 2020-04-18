#include "Billboard.h"

#include "Main/Application.h"
#include "Module/ModuleProgram.h"
#include "Module/ModuleResourceManager.h"

Billboard::Billboard(const std::string& texture_path, float width, float height) : width(width), height(height)
{
	billboard_texture = App->resources->Load<Texture>(texture_path.c_str());
	billboard_quad = App->resources->Load<Mesh>(PRIMITIVE_QUAD_PATH);
}


Billboard::~Billboard()
{
	// TODO: Check if not deleting billboard_quad causes a memory leak.
}

void Billboard::Render(const float3& position) const
{
	
	GLuint shader_program = App->program->GetShaderProgramId("Billboard");
	glUseProgram(shader_program);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, billboard_texture->opengl_texture);
	glUniform1i(glGetUniformLocation(shader_program, "billboard.texture"), 0);
	glUniform1f(glGetUniformLocation(shader_program, "billboard.width"), width);
	glUniform1f(glGetUniformLocation(shader_program, "billboard.height"), height);
	glUniform3fv(glGetUniformLocation(shader_program, "billboard.center_pos"), 1, position.ptr());

	
	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glUseProgram(0);
	
}

