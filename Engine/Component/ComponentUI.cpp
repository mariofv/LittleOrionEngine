#include "ComponentUI.h"
#include <GL/glew.h>

#include "Main/Application.h"
#include "Main/GameObject.h"

#include "Module/ModuleCamera.h"
#include "Module/ModuleEditor.h"
#include "Module/ModuleProgram.h"

#include "UI/Panel/PanelScene.h"

ComponentUI::ComponentUI() : Component(nullptr, ComponentType::UI)
{
	InitData();
}

ComponentUI::ComponentUI(GameObject * owner) : Component(owner, ComponentType::UI)
{
	InitData();
}

void ComponentUI::Render() 
{
	
	model = model.Scale(float3(size.x, -size.y, 0.0f), float3::zero);
	model.SetTranslatePart(float3(position, 0.0f));

	float4x4 projection = float4x4::D3DOrthoProjLH(0, 1, window_width, window_height);
	
	glUseProgram(shader_program);
	glUniformMatrix4fv(glGetUniformLocation(shader_program, "projection"), 1, GL_TRUE, projection.ptr());
	glUniform1i(glGetUniformLocation(shader_program, "image"), 0);
	glUniformMatrix4fv(glGetUniformLocation(shader_program, "model"), 1, GL_TRUE, model.ptr());
	glUniform3fv(glGetUniformLocation(shader_program, "spriteColor"), 1, color.ptr());
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, ui_texture);

	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
	App->cameras->scene_camera->camera_frustum.type = FrustumType::PerspectiveFrustum;
	glUseProgram(0);
}

void ComponentUI::InitData() 
{
	shader_program = App->program->GetShaderProgramId("Sprite");
	window_width = App->editor->scene_panel->scene_window_content_area_width;
	window_height = App->editor->scene_panel->scene_window_content_area_height;
	GLuint VBO;
	GLfloat vertices[] = {
		// Pos      // Tex
		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,

		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f
	};

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(vao);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}