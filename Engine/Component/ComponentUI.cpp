#include "ComponentUI.h"

#include "EditorUI/Panel/PanelScene.h"

#include "Main/Application.h"
#include "Main/GameObject.h"

#include "Module/ModuleCamera.h"
#include "Module/ModuleProgram.h"
#include "Module/ModuleResourceManager.h"
#include "Module/ModuleUI.h"

#include "ResourceManagement/Resources/Texture.h"

#include <GL/glew.h>

ComponentUI::ComponentUI(ComponentType ui_type) : Component(nullptr, ui_type)
{
	InitData();
}

ComponentUI::ComponentUI(GameObject* owner, ComponentType ui_type) : Component(owner, ui_type)
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
		glBindTexture(GL_TEXTURE_2D, 0);
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

void ComponentUI::SpecializedSave(Config& config) const
{
	config.AddUInt(ui_texture, "Texture");
	config.AddFloat3(color, "Color");
	config.AddUInt(texture_uuid, "TextureUUID");
	config.AddInt(layer, "Layer");

	UISpecializedSave(config);
}

void ComponentUI::SpecializedLoad(const Config& config)
{
	config.GetFloat3("Color", color, float3::one);
	layer = config.GetInt("Layer", 0);

	texture_uuid = config.GetUInt("TextureUUID", 0);
	ui_texture = config.GetUInt("Texture", 0);
	if(texture_uuid != 0)
	{
		SetTextureToRender(texture_uuid);
	}

	UISpecializedLoad(config);

	InitData();
}

void ComponentUI::SetTextureToRender(uint32_t texture_uuid)
{
	this->texture_uuid = texture_uuid;
	texture_to_render = App->resources->Load<Texture>(texture_uuid);
	ui_texture = texture_to_render->opengl_texture;
}

void ComponentUI::Enable()
{
	active = true;
	App->ui->SortComponentsUI();
}

void ComponentUI::Disable()
{
	active = false;
	App->ui->SortComponentsUI();
}