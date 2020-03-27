#include "Billboard.h"

#include "Main/Application.h"
#include "Module/ModuleProgram.h"
#include "Module/ModuleTexture.h"
#include "Module/ModuleResourceManager.h"

#include "ResourceManagement/ResourcesDB/CoreResources.h"

Billboard::Billboard(CoreResource quad_sprite, float width, float height) : width(width), height(height)
{
	billboard_texture = std::static_pointer_cast<Texture>(App->resources->Load((uint32_t)quad_sprite));
	billboard_quad = std::static_pointer_cast<Mesh>(App->resources->Load((uint32_t)CoreResource::QUAD));
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

	glBindVertexArray(billboard_quad->GetVAO());
	glDrawElements(GL_TRIANGLES, billboard_quad->indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glUseProgram(0);
}

