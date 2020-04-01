#include "ComponentText.h"

#include "Main/Application.h"
#include "Main/GameObject.h"

#include "Module/ModuleUI.h"

#include "EditorUI/DebugDraw.h"  


ComponentText::ComponentText() : ComponentUI(ComponentUI::UIType::TEXT)
{
	InitData();
}

ComponentText::ComponentText(GameObject * owner) : ComponentUI(owner, ComponentUI::UIType::TEXT)
{
	InitData();
}

void ComponentText::InitData()
{
	color = float3::unitZ;
	if (App->ui->glyphInit == false)
	{
		App->ui->InitGlyph();
	}
}

void ComponentText::Render(float4x4* projection)
{
	if (owner->IsEnabled() && active)
	{
		// Set OpenGL options
		//glEnable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Activate corresponding render state	
		glUseProgram(shader_program);
		glUniformMatrix4fv(glGetUniformLocation(shader_program, "projection"), 1, GL_TRUE, projection->ptr());
		glUniform1i(glGetUniformLocation(shader_program, "text"), 0);
		glUniformMatrix4fv(glGetUniformLocation(shader_program, "model"), 1, GL_TRUE, float4x4::identity.ptr());
		glUniform3fv(glGetUniformLocation(shader_program, "spriteColor"), 1, color.ptr());
		//dd::plane(owner->transform_2d.position, );
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, ui_texture);
		glBindVertexArray(vao);

		// Iterate through all characters
		std::string::const_iterator c;
		float x = 0.0f;
		float y = 0.0f;
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
			text_heigth = max(text_heigth, ypos + h);
			// Update VBO for each character
			GLfloat vertices[6][4] = {
				{ xpos,     ypos + h,   0.0, 0.0 },
				{ xpos,     ypos,       0.0, 1.0 },
				{ xpos + w, ypos,       1.0, 1.0 },

				{ xpos,     ypos + h,   0.0, 0.0 },
				{ xpos + w, ypos,       1.0, 1.0 },
				{ xpos + w, ypos + h,   1.0, 0.0 }
			};
			// Render glyph texture over quad
			glBindTexture(GL_TEXTURE_2D, ch.TextureID);
			// Update content of VBO memory
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData
			
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			// Render quad
			glDrawArrays(GL_TRIANGLES, 0, 6);
			// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
			x += (ch.Advance >> 6) * scale_factor; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
			
		}
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
		owner->transform_2d.SetSize(text_witdh, text_heigth);
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
	
}

void ComponentText::Load(const Config& config)
{
	ComponentUI::Load(config);
	config.GetString("Text", text, "");
}


	