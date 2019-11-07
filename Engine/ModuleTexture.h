#pragma once
#include "Module.h"
#include "Globals.h"
#include "Texture.h"

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

	
	const Texture* loadTexture(const char* texture_path) const;
};
