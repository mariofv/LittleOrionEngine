#include "ComponentText.h"

#include "Main/Application.h"
#include "Main/GameObject.h"

#include "Module/ModuleProgram.h"
#include "Module/ModuleResourceManager.h"
#include "Module/ModuleUI.h"

ComponentText::ComponentText() : ComponentUI(ComponentUI::UIType::TEXT)
{
	InitData();
}

ComponentText::ComponentText(GameObject * owner) : ComponentUI(owner, ComponentUI::UIType::TEXT)
{
	InitData();
	if (owner->transform_2d.is_new)
	{
		owner->transform_2d.SetPosition(&float3(0.0F, 0.0F, -1.0F));
		owner->transform_2d.SetSize(189, 26);
	}
}

void ComponentText::InitData()
{
	ComponentUI::InitData();
	shader_program = App->program->GetShaderProgramId("UI Text");
	color = float3::unitZ;
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
	float scale_factor = scale / 100;
	for (char const &c : text)
	{
		Font::Character character = font->GetCharacter(c);
		float x_pos = x + character.bearing.x * scale_factor;
		float y_pos = y - (character.glyph_size.y - character.bearing.y) * scale_factor;

		float width = character.glyph_size.x * scale_factor;
		float height = character.glyph_size.y * scale_factor;

		text_witdh = max(text_witdh, x_pos + width);
		text_heigth = max(text_heigth, abs(y_pos) + height);

		transform_2d_txt->CalculateRectMatrix(x_pos, y_pos + height, width, -height, txt_model);
			
		ComponentUI::Render(projection, &txt_model, character.glyph_texture_id, &color);
		
		x += (character.advance >> 6) * scale_factor; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))

		//following code is for future, shifting letters in y-axis (Unity-like) if text width is bigger than transform width
		if (x_pos + width > owner->transform_2d.rect.Width())
		{
			y -= text_heigth;
			x = owner->transform_2d.rect.left;
		}
	}
}

void ComponentText::Delete()
{
	ComponentUI::Delete();
}

void ComponentText::Save(Config& config) const
{
	ComponentUI::Save(config);
	config.AddString(text, "Text");
	config.AddFloat(scale, "Scale");
}

void ComponentText::Load(const Config& config)
{
	ComponentUI::Load(config);
	config.GetString("Text", text, "");
	config.GetFloat("Scale", scale);
}

void ComponentText::SetFont(uint32_t font_uuid)
{
	this->font_uuid = font_uuid;
	font = App->resources->Load<Font>(font_uuid);
}

	