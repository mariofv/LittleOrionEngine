#include "ComponentText.h"

#include "Main/Application.h"
#include "Main/GameObject.h"

#include "Module/ModuleProgram.h"
#include "Module/ModuleResourceManager.h"
#include "Module/ModuleUI.h"

#include "ResourceManagement/ResourcesDB/CoreResources.h"

ComponentText::ComponentText() : Component(ComponentType::UI_TEXT)
{
	InitData();
}

ComponentText::ComponentText(GameObject * owner) : Component(owner, ComponentType::UI_TEXT)
{
	InitData();
	SetFont((uint32_t)CoreResource::DEFAULT_FONT);
	SetFontSize(12);
}

void ComponentText::InitData()
{
	program = App->program->GetShaderProgramId("UI Text");

	GLfloat vertices[] = {
		// Pos      // Tex
		0.f, 1.f, 0.0f, 0.0f,
		1.f, 0.f, 1.0f, 1.0f,
		0.f, 0.f, 0.0f, 1.0f,

		0.f, 1.f, 0.0f, 0.0f,
		1.f, 1.f, 1.0f, 0.0f,
		1.f, 0.f, 1.0f, 1.0f
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

void ComponentText::Update()
{
	ComputeTextLines();
}

//TODO: Improve this shit
void ComponentText::Render(float4x4* projection)
{	
	if (font_uuid == 0)
	{
		return;
	}
	if (line_sizes.size() == 0)
	{
		return;
	}

	glUseProgram(program);
	glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_TRUE, projection->ptr());
	glUniform3fv(glGetUniformLocation(program, "font_color"), 1, font_color.ptr());
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(vao);


	int current_line = 0;
	
	float cursor_x = 0;
	float cursor_y = 0;
	float x = GetLineStartPosition(line_sizes[current_line]);
	float y = 0;

	// Iterate through all characters
	for (char const &c : text)
	{
		Font::Character character = font->GetCharacter(c);
		float character_size = (character.advance >> 6) * scale_factor; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
		
		float next_cursor_x = cursor_x + character_size; 
		if (next_cursor_x > owner->transform_2d.size.x)
		{
			float next_cursor_y = cursor_y + font->GetMaxHeight() * scale_factor;
			if (next_cursor_y > owner->transform_2d.size.y)
			{
				break;
			}
			else
			{
				cursor_x = 0;
				x = GetLineStartPosition(line_sizes[current_line]);

				cursor_y = font->GetMaxHeight() * scale_factor;
				y -= font->GetMaxHeight() * scale_factor;
			}
			
		}
		else
		{
			cursor_x = next_cursor_x;
		}

		float x_pos = x + character.bearing.x * scale_factor;
		float y_pos = y - (character.glyph_size.y - character.bearing.y + font->GetMaxHeight() * 0.5f) * scale_factor;

		float width = character.glyph_size.x * scale_factor;
		float height = character.glyph_size.y * scale_factor;

		float4x4 text_model_matrix = owner->transform_2d.GetGlobalModelMatrix();
		float4x4 character_model_matrix = float4x4::FromTRS(float3(x_pos, y_pos, 0.f), Quat::identity, float3(width, height, 1.f));
		float4x4 model_matrix = text_model_matrix * character_model_matrix;

		glUniform1i(glGetUniformLocation(program, "image"), 0);
		glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, model_matrix.ptr());
		glBindTexture(GL_TEXTURE_2D, character.glyph_texture_id);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		x += character_size;
	}

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
}

void ComponentText::ComputeTextLines()
{
	line_sizes.clear();

	float cursor_x = 0;
	float cursor_y = 0;

	// Iterate through all characters
	for (unsigned int i = 0; i < text.size(); ++i)
	{
		char const &c = text[i];
		Font::Character character = font->GetCharacter(c);

		float next_cursor_x = cursor_x + (character.advance >> 6) * scale_factor; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
	
		if (next_cursor_x > owner->transform_2d.size.x)
		{
			float next_cursor_y = cursor_y + font->GetMaxHeight() * scale_factor;
			if (next_cursor_y > owner->transform_2d.size.x)
			{
				return;
			}
			else
			{
				line_sizes.push_back(cursor_x);
				cursor_x = 0;

				cursor_y = next_cursor_y;
			}
		}
		else
		{
			cursor_x = next_cursor_x;
		}
		
	}

	line_sizes.push_back(cursor_x);
}

float ComponentText::GetLineStartPosition(float line_size) const
{
	switch (horizontal_alignment)
	{
	case HorizontalAlignment::LEFT:
		return 0.f;

	case HorizontalAlignment::CENTER:
		return -line_size / 2.f;

	case HorizontalAlignment::RIGHT:
		return owner->transform_2d.size.x * 0.5f - line_size;
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
	config.AddFloat(font_size, "FontSize");
	config.AddUInt((uint32_t)horizontal_alignment, "HorizontalAlignment");
}

void ComponentText::SpecializedLoad(const Config& config)
{
	config.GetString("Text", text, "");
	config.GetFloat("FontSize", font_size);
	uint32_t horizontal_alignment_uint32 = config.GetUInt("HorizontalAlignment", 0);
	horizontal_alignment = static_cast<HorizontalAlignment>(horizontal_alignment_uint32);
}

void ComponentText::SetText(const std::string& new_text)
{
	text = new_text;
	ComputeTextLines();
}

void ComponentText::SetFont(uint32_t font_uuid)
{
	this->font_uuid = font_uuid;
	font = App->resources->Load<Font>(font_uuid);
	ComputeTextLines();
}

void ComponentText::SetFontSize(int font_size)
{
	this->font_size = font_size;
	scale_factor = font_size / 64.f;
	ComputeTextLines();
}