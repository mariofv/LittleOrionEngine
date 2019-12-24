#ifndef _MODULETEXTURE_H_
#define _MODULETEXTURE_H_

#include "Module.h"
#include "Globals.h"

#include <GL/glew.h>
#include <memory>

struct SDL_Texture;
struct SDL_Renderer;
struct SDL_Rect;

class ComponentMaterial;
class ComponentLight;
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

	ComponentLight* CreateComponentLight();
	void RemoveComponentLight(ComponentLight* light_to_remove);
	
	std::shared_ptr<Texture> LoadTexture(const char* texture_path);
	GLuint  LoadCubemap(std::vector<std::string> faces) const;
	void GenerateCheckerboardTexture();

public:
	GLuint checkerboard_texture_id;
	std::vector<ComponentMaterial*> materials;
	std::vector<ComponentLight*> lights;

};

#endif //_MODULETEXTURE_H_
