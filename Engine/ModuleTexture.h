#ifndef _MODULETEXTURE_H_
#define _MODULETEXTURE_H_

#include "Module.h"
#include "Globals.h"
#include "Texture.h"

#include <GL/glew.h>

struct SDL_Texture;
struct SDL_Renderer;
struct SDL_Rect;

class ModuleTexture : public Module
{
public:
	ModuleTexture();
	~ModuleTexture();

	bool Init();
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();

	
	Texture* loadTexture(const char* texture_path) const;
};

#endif _MODULETEXTURE_H_
