#include "Font.h"

Font::Font(uint32_t uuid, std::map<GLchar, Character> characters) : Resource(uuid), characters(characters) 
{
	max_advance = 0;
	max_height = 0;

	for (auto& character : characters)
	{
		if (character.second.glyph_size.y > max_height)
		{
			max_height = static_cast<int>(character.second.glyph_size.y);
		}

		if (character.second.advance > max_advance)
		{
			max_advance = character.second.advance;
		}
	}

	Resource(initialized) = true;
}

Font::Character Font::GetCharacter(const char& c)
{
	return characters[c];
}

int Font::GetMaxHeight() const
{
	return max_height;
}

int Font::GetMaxAdvance() const
{
	return max_advance;
}