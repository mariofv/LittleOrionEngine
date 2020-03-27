#include "ComponentText.h"

#include "Main/Application.h"
#include "Main/GameObject.h"

#include "Module/ModuleCamera.h"
#include "Module/ModuleEditor.h"
#include "Module/ModuleProgram.h"
#include "Module/ModuleUI.h"

#include "EditorUI/Panel/PanelScene.h"




ComponentText::ComponentText() : Component(nullptr, ComponentType::TEXT)
{
	InitData();
}

ComponentText::ComponentText(GameObject * owner) : Component(owner, ComponentType::TEXT)
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

void ComponentText::Render()
{
	// Set OpenGL options
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//model = model.Scale(float3(size.x, -size.y, 0.0f), float3::zero);
	//model.SetTranslatePart(float3(position, 0.0f));

	float4x4 projection = float4x4::D3DOrthoProjLH(0, 1, App->ui->window_width, App->ui->window_height);
	// Activate corresponding render state	
	glUseProgram(App->ui->shader_program);
	glUniformMatrix4fv(glGetUniformLocation(App->ui->shader_program, "projection"), 1, GL_TRUE, projection.ptr());
	glUniform1i(glGetUniformLocation(App->ui->shader_program, "text"), 0);
	glUniformMatrix4fv(glGetUniformLocation(App->ui->shader_program, "model"), 1, GL_TRUE, model.ptr());
	glUniform3fv(glGetUniformLocation(App->ui->shader_program, "spriteColor"), 1, color.ptr());

	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(App->ui->VAO);

	// Iterate through all characters
	std::string::const_iterator c;
	x = position.x;
	y = position.y;
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
		glBindBuffer(GL_ARRAY_BUFFER, App->ui->VBO);
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


	