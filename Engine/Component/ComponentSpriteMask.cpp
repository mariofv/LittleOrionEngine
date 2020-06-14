#include "ComponentSpriteMask.h"

#include "Main/GameObject.h"
#include "Module/ModuleResourceManager.h"
#include "Module/ModuleProgram.h"
#include "Module/ModuleUI.h"

#include "ResourceManagement/Resources/Texture.h"

ComponentSpriteMask::ComponentSpriteMask() : Component(ComponentType::UI_SPRITE_MASK)
{
	InitData();
}

ComponentSpriteMask::ComponentSpriteMask(GameObject * owner) : Component(owner, ComponentType::UI_SPRITE_MASK)
{
	InitData();
}

ComponentSpriteMask::~ComponentSpriteMask()
{
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
}

void ComponentSpriteMask::InitData()
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

void ComponentSpriteMask::Render(float4x4* projection)
{
	if (!active)
	{
		return;
	}

	if (texture_to_render != nullptr)
	{
		float4x4 model = owner->transform_2d.GetGlobalModelMatrix()  *  float4x4::Scale(float3(size, 1.f));

		glUseProgram(program);
		glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_TRUE, projection->ptr());
		glUniform1i(glGetUniformLocation(program, "image"), 0);
		glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, model.ptr());
		glUniform4fv(glGetUniformLocation(program, "spriteColor"), 1, float3::one.ptr());
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture_to_render->opengl_texture);

		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glUseProgram(0);

	}
}

Component* ComponentSpriteMask::Clone(bool original_prefab) const
{
	ComponentSpriteMask * created_component;
	if (original_prefab)
	{
		created_component = new ComponentSpriteMask();
	}
	else
	{
		created_component = App->ui->CreateComponentUI<ComponentSpriteMask>();
	}
	*created_component = *this;
	CloneBase(static_cast<Component*>(created_component));
	return created_component;
};

void ComponentSpriteMask::Copy(Component* component_to_copy) const
{
	*component_to_copy = *this;
	*static_cast<ComponentSpriteMask*>(component_to_copy) = *this;
}


void ComponentSpriteMask::Delete()
{
	App->ui->RemoveComponentUI(this);
}

void ComponentSpriteMask::SpecializedSave(Config& config) const
{
	config.AddBool(render_mask, "RenderMask");
	config.AddBool(inverted_mask, "InvertedMask");
	config.AddFloat2(size, "Size");
	config.AddUInt(texture_uuid, "TextureUUID");
}

void ComponentSpriteMask::SpecializedLoad(const Config& config)
{
	render_mask = config.GetBool("RenderMask", false);
	inverted_mask = config.GetBool("InvertedMask", false);
	config.GetFloat2("Size", size, float2(100.f, 100.f));
	texture_uuid = config.GetUInt("TextureUUID", 0);
	if (texture_uuid != 0)
	{
		SetTextureToRender(texture_uuid);
	}
}

void ComponentSpriteMask::SetTextureToRender(uint32_t texture_uuid)
{
	this->texture_uuid = texture_uuid;
	texture_to_render = App->resources->Load<Texture>(texture_uuid);
	texture_aspect_ratio = (float)texture_to_render->width / texture_to_render->height;
}

void ComponentSpriteMask::SetNativeSize()
{
	size = float2(texture_to_render->width, texture_to_render->height);
}