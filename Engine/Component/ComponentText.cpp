#include "ComponentText.h"

#include "Main/Application.h"
#include "Main/GameObject.h"

#include "Module/ModuleProgram.h"
#include "Module/ModuleUI.h"

#include "EditorUI/DebugDraw.h"  


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

ComponentText::~ComponentText()
{
	ComponentUI::~ComponentUI();
}

void ComponentText::InitData()
{
	ComponentUI::InitData();
	shader_program = App->program->GetShaderProgramId("UI Text");
	color = float3::unitZ;
	if (App->ui->glyphInit == false)
	{
		App->ui->InitGlyph();
	}
}

void ComponentText::Render(float4x4* projection)
{
	ComponentTransform2D* transform_2d_txt = &owner->transform_2d;
	float4x4 txt_model;

	if (owner->IsEnabled() && active)
	{
		// Iterate through all characters
		std::string::const_iterator c;
		float x = owner->transform_2d.rect.left;
		float y = owner->transform_2d.rect.top;
		float text_witdh = 0;
		float text_heigth = 0;
		float scale_factor = scale / 100;
		for (c = text.begin(); c != text.end(); c++)
		{
			Character ch = App->ui->Characters[*c];
			GLfloat xpos = x + ch.Bearing.x * scale_factor;
			GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale_factor;

			GLfloat w = ch.Size.x * scale_factor;
			GLfloat h = ch.Size.y * scale_factor;

			text_witdh = max(text_witdh, xpos + w);
			text_heigth = max(text_heigth, abs(ypos) + h);

			transform_2d_txt->CalculateRectMatix(xpos, ypos + h, w, -h, &txt_model);
			
			ComponentUI::Render(projection, &txt_model, ch.TextureID, &color);
		
			x += (ch.Advance >> 6) * scale_factor; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))

			//following code is for future, shifting letters in y-axis (Unity-like) if text width is bigger than transform width
			if (xpos + w > owner->transform_2d.rect.Width())
			{
				y -= text_heigth;
				x = owner->transform_2d.rect.left;
			}
			
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


	