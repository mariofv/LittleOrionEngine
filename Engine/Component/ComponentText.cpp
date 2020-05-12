#include "ComponentText.h"

#include "Main/Application.h"
#include "Main/GameObject.h"

#include "Module/ModuleProgram.h"
#include "Module/ModuleResourceManager.h"
#include "Module/ModuleUI.h"

ComponentText::ComponentText() : Component(ComponentType::UI_TEXT)
{
	InitData();
}

ComponentText::ComponentText(GameObject * owner) : Component(owner, ComponentType::UI_TEXT)
{
	InitData();
}

void ComponentText::InitData()
{
	program = App->program->GetShaderProgramId("UI Text");

	GLfloat vertices[] = {
		// Pos      // Tex
		-0.5f, 0.5f, 0.0f, 0.0f,
		0.5f, -0.5f, 1.0f, 1.0f,
		-0.5f, -0.5f, 0.0f, 1.0f,

		-0.5f, 0.5f, 0.0f, 0.0f,
		0.5f, 0.5f, 1.0f, 0.0f,
		0.5f, -0.5f, 1.0f, 1.0f
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

void ComponentText::Render(float4x4* projection)
{
	
	if (font_uuid == 0)
	{
		return;
	}
	
	ComponentTransform2D* transform_2d_txt = &owner->transform_2d;
	float4x4 txt_model;

	// Iterate through all characters

	float x = owner->transform_2d.rect.left;
	float y = owner->transform_2d.rect.top;
		
	float text_witdh = 0;
	float text_heigth = 0;

	float scale_factor = scale / 16.f;

	for (char const &c : text)
	{
		Font::Character character = font->GetCharacter(c);
		float x_pos = x + character.bearing.x * scale_factor;
		float y_pos = y - (character.glyph_size.y - character.bearing.y) * scale_factor;

		float width = character.glyph_size.x * scale_factor;
		float height = character.glyph_size.y * scale_factor;

		text_witdh = max(text_witdh, x_pos + width);
		text_heigth = max(text_heigth, abs(y_pos) + height);

		float4x4 text_model = owner->transform_2d.GetGlobalModelMatrix();
		float4x4 glyph_model = float4x4::FromTRS(float3(x_pos, y_pos, 0.f), Quat::identity, float3(width, height, 1.f));
		float4x4 model = text_model * glyph_model;

		glUseProgram(program);
		glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_TRUE, projection->ptr());
		glUniform1i(glGetUniformLocation(program, "image"), 0);
		glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, model.ptr());
		float3 color = float3::one;
		glUniform3fv(glGetUniformLocation(program, "font_color"), 1, color.ptr());
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, character.glyph_texture_id);

		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glUseProgram(0);
		
		x += (character.advance >> 6) * scale_factor; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))

		//following code is for future, shifting letters in y-axis (Unity-like) if text width is bigger than transform width
		if (x_pos + width > owner->transform_2d.size.x)
		{
			y -= text_heigth;
			x = owner->transform_2d.rect.left;
		}
	}
}

Component* ComponentText::Clone(bool original_prefab) const
{
	ComponentText* created_component;
	if (original_prefab)
	{
		created_component = new ComponentText();
	}
	else
	{
		created_component = App->ui->CreateComponentUI<ComponentText>();
	}
	*created_component = *this;
	return created_component;
};

void ComponentText::Copy(Component* component_to_copy) const
{
	*component_to_copy = *this;
	*static_cast<ComponentText*>(component_to_copy) = *this;
}


void ComponentText::Delete()
{
	App->ui->RemoveComponentUI(this);
}

void ComponentText::SpecializedSave(Config& config) const
{
	config.AddString(text, "Text");
	config.AddFloat(scale, "Scale");
}

void ComponentText::SpecializedLoad(const Config& config)
{
	config.GetString("Text", text, "");
	config.GetFloat("Scale", scale);
}

void ComponentText::SetFont(uint32_t font_uuid)
{
	this->font_uuid = font_uuid;
	font = App->resources->Load<Font>(font_uuid);
}

	