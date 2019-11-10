#pragma once
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

	
	const bool loadTexture(const char* texture_path, const GLuint &texture) const;
};
