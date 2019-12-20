#ifndef _MODULEMODELLOADER_H_
#define _MODULEMODELLOADER_H_

#include "Module.h"
#include "Globals.h"
#include "Texture.h"

#include <GL/glew.h>
#include "assimp/LogStream.hpp"
#include "assimp/Logger.hpp"

#include <vector>
#include <string>

class ComponentMesh;
class GameObject;

struct aiMesh;
struct aiMaterial;
struct aiScene;
struct aiNode;

class AssimpStream : public Assimp::LogStream
{
	public:
		AssimpStream() = default;
		AssimpStream(const unsigned int severety):severety(severety){}
		~AssimpStream() = default;

		void write(const char* message)
		{
			switch (severety)
			{
			case Assimp::Logger::Debugging:
				MYASSIMP_LOG_INFO("%s", message);
				break;
			case Assimp::Logger::Info:
				MYASSIMP_LOG_INFO("%s", message);
				break;
			case Assimp::Logger::Err:
				MYASSIMP_LOG_ERROR("%s", message);
				break;
			case Assimp::Logger::Warn:
				MYASSIMP_LOG_INIT("%s", message); // Actually not an itialization entry, I use this type of entry because the yellow color
				break;

			}

		}

	public:
		unsigned int severety = 0;
};


class ModuleModelLoader : public Module
{
public:
	ModuleModelLoader() = default;
	~ModuleModelLoader() = default;

	bool Init();
	bool CleanUp();

	GameObject* LoadModel(const char *new_model_file_path);

private:
	void UnloadCurrentModel();
	void LoadNode(GameObject *parent_node, const std::string model_base_path);

	void LoadMeshData(const aiMesh *mesh, ComponentMesh *mesh_component) const;
	std::shared_ptr<Texture> LoadMaterialData(const aiMaterial *material, const std::string model_base_path);

	std::string GetModelBasePath(const char *model_file_path) const;
	std::string GetTextureFileName(const char *texture_file_path) const;

public:
	const aiScene* scene = nullptr;
};

#endif //_MODULEMODELLOADER_H_