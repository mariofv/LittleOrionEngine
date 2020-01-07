#ifndef _MODULEMODELLOADER_H_
#define _MODULEMODELLOADER_H_

#include "Module.h"
#include "Globals.h"
#include "Texture.h"
#include "ModuleFileSystem.h"

#include <GL/glew.h>
#include <vector>
#include <string>
#include <memory>

class ComponentMesh;
class GameObject;

class ModuleModelLoader : public Module
{
public:
	ModuleModelLoader() = default;
	~ModuleModelLoader() = default;

	bool Init();
	GameObject* LoadModel(const char *new_model_file_path);

	GameObject* LoadCoreModel(const char* new_model_file_path);

private:
	void LoadNode(GameObject *parent_node, const std::shared_ptr<ModuleFileSystem::File> & model_base_path);
};

#endif //_MODULEMODELLOADER_H_