#include "ComponentText.h"

#include "Main/Application.h"
#include "Main/GameObject.h"

#include "Module/ModuleCamera.h"
#include "Module/ModuleEditor.h"
#include "Module/ModuleProgram.h"
#include "Module/ModuleUI.h"

#include "EditorUI/Panel/PanelScene.h"


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
	if (App->ui->glyphInit == false)
	{
		App->ui->InitGlyph();
	}
}

void ComponentText::Render(float4x4* projection)
{
	// Set OpenGL options
	/*glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);*/

	// Activate corresponding render state	
	glUseProgram(shader_program);
	glUniformMatrix4fv(glGetUniformLocation(shader_program, "projection"), 1, GL_TRUE, projection->ptr());
	glUniform1i(glGetUniformLocation(shader_program, "text"), 0);
	glUniformMatrix4fv(glGetUniformLocation(shader_program, "model"), 1, GL_TRUE, owner->transform_2d.scale_matrix.ptr());
	glUniform3fv(glGetUniformLocation(shader_program, "spriteColor"), 1, color.ptr());

	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(vao);

	// Iterate through all characters
	std::string::const_iterator c;
	x = 0;
	y = 0;

	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = App->ui->Characters[*c];
		
		GLfloat xpos = x + ch.Bearing.x * scale;
		GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;
		
		GLfloat w = ch.Size.x * scale;
		GLfloat h = ch.Size.y * scale;
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
		x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void ComponentText::Delete()
{
	ComponentUI::Delete();
}



void ComponentText::Save(Config& config) const
{
	ComponentUI::Save(config);
	
}

void ComponentText::Load(const Config& config)
{
	ComponentUI::Load(config);
}


	