#include "ComponentImage.h"

#include "Main/GameObject.h"
#include "Module/ModuleResourceManager.h"
#include "Module/ModuleProgram.h"

#include "ResourceManagement/Resources/Texture.h"

ComponentImage::ComponentImage() : ComponentUI(ComponentType::UI_IMAGE)
{
	InitData();
}

ComponentImage::ComponentImage(GameObject * owner) : ComponentUI(owner, ComponentType::UI_IMAGE)
{
	InitData();
}

ComponentImage::~ComponentImage()
{
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
}

void ComponentImage::InitData()
{
	program = App->program->GetShaderProgramId("Sprite");

	GLfloat vertices[] = {
		// Pos      // Tex
		-0.5f, 0.5f, 0.0f, 1.0f,
		0.5f, -0.5f, 1.0f, 0.0f,
		-0.5f, -0.5f, 0.0f, 0.0f,

		-0.5f, 0.5f, 0.0f, 1.0f,
		0.5f, 0.5f, 1.0f, 1.0f,
		0.5f, -0.5f, 1.0f, 0.0f
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


void ComponentImage::Render(float4x4* projection)
{
	Render(projection, &owner->transform_2d.GetSizedGlobalModelMatrix());
}

void ComponentImage::Render(float4x4* projection, float4x4* model)
{
	if (texture_to_render != nullptr)
	{
		glUseProgram(program);
		glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_TRUE, projection->ptr());
		glUniform1i(glGetUniformLocation(program, "image"), 0);
		glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, model->ptr());
		glUniform3fv(glGetUniformLocation(program, "spriteColor"), 1, color.ptr());
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture_to_render->opengl_texture);

		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glUseProgram(0);

	}
}

void ComponentImage::UISpecializedSave(Config& config) const
{
	config.AddFloat3(color, "Color");
	config.AddUInt(texture_uuid, "TextureUUID");
}

void ComponentImage::UISpecializedLoad(const Config& config)
{
	config.GetFloat3("Color", color, float3::one);
	texture_uuid = config.GetUInt("TextureUUID", 0);
	if (texture_uuid != 0)
	{
		SetTextureToRender(texture_uuid);
	}
}

void ComponentImage::SetTextureToRender(uint32_t texture_uuid)
{
	this->texture_uuid = texture_uuid;
	texture_to_render = App->resources->Load<Texture>(texture_uuid);
}
