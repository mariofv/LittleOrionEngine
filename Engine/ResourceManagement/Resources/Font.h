#ifndef _FONT_H_
#define _FONT_H_

#include "Resource.h"

#include "ResourceManagement/Manager/FontManager.h"

#include <map>
#include <MathGeoLib.h>
#include <GL/glew.h>

struct SDL_Renderer;

class Font : public Resource
{
public:
	struct Character 
	{
		GLuint glyph_texture_id;
		float2 glyph_size;
		float2 bearing;  // Offset from baseline to left/top of glyph
		GLuint advance;    // Horizontal offset to advance to next glyph
	};

	Font() = default;
	Font(uint32_t uuid) : Resource(uuid) {};
	Font(uint32_t uuid, std::map<GLchar, Character> characters) : Resource(uuid), characters(characters) {};

	~Font() = default;

	Character GetCharacter(const char& c);

private:
	std::map<GLchar, Character> characters;
};

namespace ResourceManagement
{
	template<>
	static std::shared_ptr<Font> Load(uint32_t uuid, const FileData& resource_data)
	{
		return FontManager::Load(uuid, resource_data);
	};
}

#endif // !_RESOURCE_H_

