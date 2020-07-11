#include "FontManager.h"

#include "Main/Application.h"
#include "Module/ModuleEditor.h"
#include "Module/ModuleFileSystem.h"
#include "Module/ModuleResourceManager.h"
#include "ResourceManagement/Resources/Font.h"

#include <Brofiler/Brofiler.h>
#include <map>
#include <ft2build.h>

#include FT_FREETYPE_H

std::shared_ptr<Font> FontManager::Load(uint32_t uuid, const FileData& resource_data)
{
	std::map<GLchar, Font::Character> font_characters;

	FT_Library ft;
	FT_Face face;

	// All functions return a value different than 0 whenever an error occurred
	if (FT_Init_FreeType(&ft))
	{
		RESOURCES_LOG_ERROR("Could not init FreeType Library")
		return nullptr;
	}

	// Load font as face
	if (FT_New_Memory_Face(ft, (FT_Byte*)resource_data.buffer, resource_data.size, 0, &face))
	{
		RESOURCES_LOG_ERROR("Failed to load font %u from memory")
		FT_Done_FreeType(ft);
		return nullptr;
	}

	// Set size to load glyphs as
	//FT_Set_Pixel_Sizes(face, 0, 16);
	FT_Set_Pixel_Sizes(face, 0, 64);

	// Disable byte-alignment restriction
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	
	for (GLubyte character = 0; character < 128; character++)
	{
		if (FT_Load_Char(face, character, FT_LOAD_RENDER))
		{
			RESOURCES_LOG_ERROR("Failed to load Glyph")
			continue;
		}

		GLuint glyph_texture;
		glGenTextures(1, &glyph_texture);
		glBindTexture(GL_TEXTURE_2D, glyph_texture);
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

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		Font::Character loaded_character
		{
			glyph_texture,
			float2(static_cast<float>(face->glyph->bitmap.width), static_cast<float>(face->glyph->bitmap.rows)),
			float2(static_cast<float>(face->glyph->bitmap_left), static_cast<float>(face->glyph->bitmap_top)),
			face->glyph->advance.x
		};
		font_characters.insert(std::pair<GLchar, Font::Character>(character, loaded_character));
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	std::shared_ptr<Font> loaded_font = std::make_shared<Font>(uuid, font_characters);

	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	return loaded_font;
}