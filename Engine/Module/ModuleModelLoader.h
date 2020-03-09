#ifndef _MODULEMODELLOADER_H_
#define _MODULEMODELLOADER_H_

#include "Module.h"
#include "Main/Globals.h"
#include "ResourceManagement/Resources/Texture.h"
#include "ModuleFileSystem.h"

#include <GL/glew.h>
#include <vector>
#include <string>


class ComponentMesh;
class GameObject;
class File;
class Config;
class ModuleModelLoader : public Module
{
public:
	ModuleModelLoader() = default;
	~ModuleModelLoader() = default;

	bool Init() override;
	GameObject* LoadModel(const char *new_model_file_path) const;
	GameObject* LoadCoreModel(const char* new_model_file_path) const;

private:
	void LoadNode(GameObject *parent_node, const Config & node_config, std::vector<std::string> & already_loaded_skeleton) const;

};

#endif //_MODULEMODELLOADER_H_