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
#include <thread>
#include <atomic>

class ComponentMesh;
class GameObject;
class File;
class ModuleModelLoader : public Module
{
public:
	ModuleModelLoader() = default;
	~ModuleModelLoader() = default;

	bool Init() override;
	bool CleanUp() override;
	GameObject* LoadModel(const char *new_model_file_path);
	GameObject* LoadCoreModel(const char* new_model_file_path);

private:
	void LoadNode(GameObject *parent_node, const std::shared_ptr<File> & model_base_path);
	std::thread importing;
public:
	struct ThreadComunication
	{
		std::atomic_bool stop_thread = false;
		std::atomic_uint importing_hash = 0;
	} thread_comunication;
};

#endif //_MODULEMODELLOADER_H_