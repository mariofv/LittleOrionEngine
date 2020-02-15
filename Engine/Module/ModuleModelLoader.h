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
class ModuleModelLoader : public Module
{
public:
	ModuleModelLoader() = default;
	~ModuleModelLoader() = default;

	bool Init() override;
	GameObject* LoadModel(const char *new_model_file_path) const;
	GameObject* LoadCoreModel(const char* new_model_file_path) const;

private:
	void LoadNode(GameObject *parent_node, const std::shared_ptr<File> & model_base_path) const;

};

#endif //_MODULEMODELLOADER_H_