#ifndef _MODULETEXTURE_H_
#define _MODULETEXTURE_H_

#include "Module.h"
#include "Main/Globals.h"

#include <GL/glew.h>
#include <memory>

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

	bool Init() override;
	bool CleanUp() override;

	ComponentMaterial* CreateComponentMaterial();
	void RemoveComponentMaterial(ComponentMaterial* material_to_remove);
	
	std::shared_ptr<Texture> LoadTexture(const char* texture_path);
	GLuint LoadCubemap(const std::vector<std::string> & faces) const;
public:
	GLuint checkerboard_texture_id;
	GLuint whitefall_texture_id;
	std::vector<ComponentMaterial*> materials;

	const static int generate_texture_height = 64;
	const static int generate_texture_width = 64;

private:

	enum class PATRON
	{
		CHECKBOARD,
		WHITE
	};
	void GenerateTexture(GLuint &texture_id_to_store, PATRON color_id);
	GLubyte GetColor(size_t i, size_t j, PATRON color_id) const;


};

#endif //_MODULETEXTURE_H_
