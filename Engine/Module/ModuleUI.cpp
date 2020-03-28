#include "Component/ComponentCamera.h"
#include "Component/ComponentCanvas.h"
#include "Component/ComponentUI.h"
#include "Component/ComponentText.h"

#include "Main/Globals.h"
#include "Main/Application.h"

#include "Module/ModuleWindow.h"
#include "Module/ModuleEditor.h"
#include "Module/ModuleProgram.h"

#include "ModuleUI.h"
#include "SDL/SDL.h"


// Called before render is available
bool ModuleUI::Init()
{
	APP_LOG_SECTION("************ Module UI Init ************");

	render = SDL_CreateRenderer(App->window->window, -1, SDL_RENDERER_ACCELERATED);
	return true;
}

// Called every draw update
update_status ModuleUI::Update()
{
	return update_status::UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleUI::CleanUp()
{
	return true;
}

void ModuleUI::Render(const ComponentCamera* camera)
{
	for (auto &canvas : canvases)
	{
		if (canvas->IsEnabled())
		{
			//canvas->Render(camera);
		}
	}

	for (auto &ui : ui_elements)
	{
		ui->Render();
	}

	for (auto &txt : ui_texts)
	{
		if (App->ui->glyphInit == false)
		{
			InitGlyph();
			txt->Render();
		}
		else
		{
			txt->Render();
		}
	}
}

ComponentCanvas* ModuleUI::CreateComponentCanvas()
{
	ComponentCanvas * new_canvas = new ComponentCanvas();
	canvases.push_back(new_canvas);
	return new_canvas;
}

void ModuleUI::RemoveComponentCanvas(ComponentCanvas* canvas_to_remove)
{
	auto it = std::find(canvases.begin(), canvases.end(), canvas_to_remove);
	if (it != canvases.end())
	{
		delete *it;
		canvases.erase(it);
	}
}

ComponentUI* ModuleUI::CreateComponentUI()
{
	ComponentUI* new_ui = new ComponentUI();
	ui_elements.push_back(new_ui);
	return new_ui;
}

void ModuleUI::RemoveComponentUI(ComponentUI* ui_to_remove)
{
	auto it = std::find(ui_elements.begin(), ui_elements.end(), ui_to_remove);
	if (it != ui_elements.end())
	{
		delete *it;
		ui_elements.erase(it);
	}
}

ComponentText* ModuleUI::CreateComponentText()
{
	ComponentText* new_txt = new ComponentText();
	ui_texts.push_back(new_txt);
	return new_txt;
}

void ModuleUI::RemoveComponentText(ComponentText* txt_to_remove)
{
	auto it = std::find(ui_texts.begin(), ui_texts.end(), txt_to_remove);
	if (it != ui_texts.end())
	{
		delete *it;
		ui_texts.erase(it);
	}
}

void ModuleUI::InitGlyph()
{
	// All functions return a value different than 0 whenever an error occurred
	if (FT_Init_FreeType(&ft))
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

	// Load font as face
	if (FT_New_Face(ft, "Assets/Fonts/arial.ttf", 0, &face))
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

	// Set size to load glyphs as
	FT_Set_Pixel_Sizes(face, 0, 48);

	// Disable byte-alignment restriction
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	//
	shader_program = App->program->GetShaderProgramId("Sprite");
	window_width = App->editor->scene_panel->scene_window_content_area_width;
	window_height = App->editor->scene_panel->scene_window_content_area_height;
	//

	// Load first 128 characters of ASCII set
	for (GLubyte c = 0; c < 128; c++)
	{
		// Load character glyph 
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}
		// Generate texture

		glGenTextures(1, &text_texture);
		glBindTexture(GL_TEXTURE_2D, text_texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);
		// Set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// Now store character for later use
		Character character = {
			text_texture,
			float2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			float2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};
		Characters.insert(std::pair<GLchar, Character>(c, character));
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	// Destroy FreeType once we're finished
	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	glyphInit = true;
}

//Guardar aquí todos los component canvas (crear, destruir y guardar)
//Cuando se hace el render de los canvas, añadir un render a este modulo que renderice todos los canvas
//