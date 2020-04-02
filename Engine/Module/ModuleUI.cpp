#include "Component/ComponentCamera.h"
#include "Component/ComponentCanvas.h"
#include "Component/ComponentImage.h"
#include "Component/ComponentProgressBar.h"
#include "Component/ComponentUI.h"
#include "Component/ComponentText.h"
#include "Component/ComponentButton.h"

#include "GL/glew.h"
#include "Main/Globals.h"
#include "Main/Application.h"
#include "Main/GameObject.h"

#include "ModuleEditor.h"
#include "ModuleScene.h"
#include "ModuleUI.h"
#include "ModuleWindow.h"


#include "SDL/SDL.h"


// Called before render is available
bool ModuleUI::Init()
{
	APP_LOG_SECTION("************ Module UI Init ************");

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
#if GAME
	window_width = App->window->GetWidth();
	window_height = App->window->GetHeight();
#else
	window_width = App->editor->scene_panel->scene_window_content_area_width;
	window_height = App->editor->scene_panel->scene_window_content_area_height;
#endif
	float4x4 projection = float4x4::D3DOrthoProjLH(-1, MAX_NUM_LAYERS, window_width, window_height);
	if (main_canvas != nullptr)
	{
		RenderUIGameObject(main_canvas->owner, &projection);
	}
}

void  ModuleUI::RenderUIGameObject(GameObject* parent, float4x4* projection)
{
	for (auto child : parent->children)
	{
		ComponentUI* ui = static_cast<ComponentUI*>(child->GetComponent(Component::ComponentType::UI));
		if (ui)
		{
			ui->Render(projection);
		}
		RenderUIGameObject(child, projection);
	}
}

ComponentUI* ModuleUI::CreateComponentUI(ComponentUI::UIType type, GameObject* owner)
{
	ComponentUI* new_ui = nullptr;
	switch (type)
	{
		case ComponentUI::UIType::CANVAS:
			if (main_canvas == nullptr)
			{
				main_canvas = new ComponentCanvas(owner);
			}
			new_ui = main_canvas;
			break;
		case ComponentUI::UIType::IMAGE:
			new_ui = new ComponentImage(owner);
			break;
		case ComponentUI::UIType::TEXT:
			new_ui = new ComponentText(owner);
			break;
		case ComponentUI::UIType::BUTTON:
			new_ui = new ComponentButton(owner);
			break;
		case ComponentUI::UIType::PROGRESSBAR:
			new_ui = new ComponentProgressBar(owner);
			break;
	}
	if(new_ui) 
	{
		ui_elements.push_back(new_ui);
	}
	return new_ui;
}

void ModuleUI::RemoveComponentUI(ComponentUI* ui_to_remove)
{
	auto it = std::find(ui_elements.begin(), ui_elements.end(), ui_to_remove);
	if (*it == main_canvas)
	{
		main_canvas = nullptr;
	}
	if (it != ui_elements.end())
	{
		delete *it;
		ui_elements.erase(it);
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
	//FT_Set_Pixel_Sizes(face, 0, 16);
	FT_Set_Char_Size(
		face,    /* handle to face object           */
		0,       /* char_width in 1/64th of points  */
		16 * 64,   /* char_height in 1/64th of points */
		1920,     /* horizontal device resolution    */
		1080);   /* vertical device resolution      */
	// Disable byte-alignment restriction
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

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

//Guardar aqu� todos los component canvas (crear, destruir y guardar)
//Cuando se hace el render de los canvas, a�adir un render a este modulo que renderice todos los canvas
//