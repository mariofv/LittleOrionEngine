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
class MaterialImporter;

class ModuleTexture : public Module
{
public:
	ModuleTexture() = default;
	~ModuleTexture() = default;

	bool Init();
	bool CleanUp();

	ComponentMaterial* CreateComponentMaterial();
	void RemoveComponentMaterial(ComponentMaterial* material_to_remove);
	
	Texture* LoadTexture(const char* texture_path) const;
	GLuint  LoadCubemap(std::vector<std::string> faces) const;
	void GenerateCheckerboardTexture();

public:
	GLuint checkerboard_texture_id;
	std::vector<ComponentMaterial*> materials;

};

#endif //_MODULETEXTURE_H_
