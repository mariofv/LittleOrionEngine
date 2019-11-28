#ifndef _MODULETEXTURE_H_
#define _MODULETEXTURE_H_

#include "Module.h"
#include "Globals.h"

#include <GL/glew.h>

struct SDL_Texture;
struct SDL_Renderer;
struct SDL_Rect;

class ComponentMaterial;
class Texture;

class ModuleTexture : public Module
{
public:
	ModuleTexture() = default;
	~ModuleTexture() = default;

	bool Init();
	bool CleanUp();

	ComponentMaterial* CreateComponentMaterial() const;
	
	Texture* LoadTexture(const char* texture_path) const;
};

#endif //_MODULETEXTURE_H_
