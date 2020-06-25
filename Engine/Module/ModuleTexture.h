#ifndef _MODULETEXTURE_H_
#define _MODULETEXTURE_H_

#include "Module.h"
#include "Main/Globals.h"

#include <GL/glew.h>
#include <memory>

struct SDL_Texture;
struct SDL_Renderer;
struct SDL_Rect;

class Texture;
class MaterialImporter;

class ModuleTexture : public Module
{
public:
	ModuleTexture() = default;
	~ModuleTexture() = default;

	bool Init() override;
	bool CleanUp() override;

public:
	GLuint checkerboard_texture_id = 0;
	GLuint whitefall_texture_id = 0;
	GLuint blackfall_texture_id = 0;

	const static int generate_texture_height = 64;
	const static int generate_texture_width = 64;

private:

	enum class PATRON
	{
		CHECKBOARD,
		WHITE,
		BLACK
	};

	void GenerateTexture(GLuint &texture_id_to_store, PATRON color_id);
	GLubyte GetColor(size_t i, size_t j, PATRON color_id) const;


};

#endif //_MODULETEXTURE_H_
