#include "ComponentImage.h"

#include "Main/GameObject.h"
#include "Module/ModuleResourceManager.h"
#include "Module/ModuleProgram.h"
#include "Module/ModuleUI.h"

#include "ResourceManagement/Resources/Texture.h"

ComponentImage::ComponentImage() : Component(ComponentType::UI_IMAGE)
{
	InitData();
}

ComponentImage::ComponentImage(GameObject * owner) : Component(owner, ComponentType::UI_IMAGE)
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
	if (!active)
	{
		return;
	}

	if (texture_to_render != nullptr)
	{
		ScaleOp aspect_ratio_scaling;

		if (preserve_aspect_ratio)
		{
			if (owner->transform_2d.size.x / texture_aspect_ratio > owner->transform_2d.size.y)
			{
				aspect_ratio_scaling = float4x4::Scale(float3(owner->transform_2d.size.y * texture_aspect_ratio, owner->transform_2d.size.y, 1.f));
			}
			else
			{
				aspect_ratio_scaling = float4x4::Scale(float3(owner->transform_2d.size.x, owner->transform_2d.size.x / texture_aspect_ratio, 1.f));
			}
		}
		else
		{
			aspect_ratio_scaling = float4x4::Scale(float3(owner->transform_2d.size, 1.f));
		}
		float4x4 model = owner->transform_2d.GetGlobalModelMatrix()  * aspect_ratio_scaling;

		if (program == 0)
		{
			program = App->program->UseProgram("Sprite");
		}
		else
		{
			glUseProgram(program);
		}

		glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_TRUE, projection->ptr());
		glUniform1i(glGetUniformLocation(program, "image"), 0);
		glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, model.ptr());
		glUniform4fv(glGetUniformLocation(program, "spriteColor"), 1, color.ptr());
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture_to_render->opengl_texture);

		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glUseProgram(0);

	}
}

Component* ComponentImage::Clone(bool original_prefab) const
{
	ComponentImage * created_component;
	if (original_prefab)
	{
		created_component = new ComponentImage();
	}
	else
	{
		created_component = App->ui->CreateComponentUI<ComponentImage>();
	}
	*created_component = *this;
	CloneBase(static_cast<Component*>(created_component));
	return created_component;
};

void ComponentImage::Copy(Component* component_to_copy) const
{
	*component_to_copy = *this;
	*static_cast<ComponentImage*>(component_to_copy) = *this;
}


void ComponentImage::Delete()
{
	App->ui->RemoveComponentUI(this);
}

void ComponentImage::SpecializedSave(Config& config) const
{
	config.AddColor(color, "ImageColor");
	config.AddUInt(texture_uuid, "TextureUUID");
	config.AddBool(preserve_aspect_ratio, "PreserveAspectRatio");
}

void ComponentImage::SpecializedLoad(const Config& config)
{
	config.GetColor("ImageColor", color, float4::one);
	texture_uuid = config.GetUInt32("TextureUUID", 0);
	preserve_aspect_ratio = config.GetBool("PreserveAspectRatio", false);
	if (texture_uuid != 0)
	{
		SetTextureToRender(texture_uuid);
	}
}

void ComponentImage::SetTextureToRender(uint32_t texture_uuid)
{
	this->texture_uuid = texture_uuid;
	texture_to_render = App->resources->Load<Texture>(texture_uuid);
	if(texture_to_render != nullptr)
		texture_aspect_ratio = (float)texture_to_render->width / texture_to_render->height;
}

void ComponentImage::SetColor(float4 color)
{
	this->color = color;
}

void ComponentImage::SetNativeSize() const
{
	owner->transform_2d.SetSize(float2((float)texture_to_render->width, (float)texture_to_render->height));
}