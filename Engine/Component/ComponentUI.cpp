#include "ComponentUI.h"
#include <GL/glew.h>

#include "Main/Application.h"
#include "Main/GameObject.h"

#include "Module/ModuleCamera.h"
#include "Module/ModuleProgram.h"
#include "Module/ModuleUI.h"

#include "EditorUI/Panel/PanelScene.h"

ComponentUI::ComponentUI(UIType ui_type) : Component(nullptr, ComponentType::UI), ui_type(ui_type)
{
	InitData();
}

ComponentUI::ComponentUI(GameObject * owner, UIType ui_type) : Component(owner, ComponentType::UI), ui_type(ui_type)
{
	InitData();
}

ComponentUI::~ComponentUI()
{
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
}

void ComponentUI::Render(float4x4* projection)
{
	Render(projection, &owner->transform_2d.rect_matrix, ui_texture, &color);
}

void ComponentUI::Render(float4x4* projection, float4x4* model, unsigned int texture, float3* color)
{	
	if(owner->IsEnabled() && active)
	{
		glUseProgram(shader_program);
		glUniformMatrix4fv(glGetUniformLocation(shader_program, "projection"), 1, GL_TRUE, projection->ptr());
		glUniform1i(glGetUniformLocation(shader_program, "image"), 0);
		glUniformMatrix4fv(glGetUniformLocation(shader_program, "model"), 1, GL_TRUE, model->ptr());
		glUniform3fv(glGetUniformLocation(shader_program, "spriteColor"), 1, color->ptr());
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);

		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
		glUseProgram(0);
	}
}

void ComponentUI::InitData()
{
	shader_program = App->program->GetShaderProgramId("Sprite");
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
	glGenBuffers(1, &vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(vao);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void ComponentUI::Delete()
{
	App->ui->RemoveComponentUI(this);
}

void ComponentUI::Save(Config& config) const
{
	config.AddUInt(UUID, "UUID");
	config.AddBool(active, "Active");
	config.AddUInt((unsigned int)type, "ComponentType");
	config.AddUInt((unsigned int)ui_type, "UIType");
	config.AddUInt(ui_texture, "Texture");
	config.AddFloat3(color, "Color");
}

void ComponentUI::Load(const Config& config)
{
	UUID = config.GetUInt("UUID", 0);
	active = config.GetBool("Active", true);
	ui_texture = config.GetUInt("Texture", 0);
	config.GetFloat3("Color", color, float3::one);
	InitData();
}