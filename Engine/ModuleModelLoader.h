#ifndef _MODULEMODELLOADER_H_
#define _MODULEMODELLOADER_H_

#include "Module.h"
#include "Globals.h"
#include "Model.h"
#include "Mesh.h"
#include "BoundingBox.h"
#include "Texture.h"

#include <GL/glew.h>
#include "assimp/LogStream.hpp"
#include "assimp/Logger.hpp"

#include <vector>
#include <string>

struct aiMesh;
struct aiMaterial;
struct aiScene;

class AssimpStream : public Assimp::LogStream
{
public:
	AssimpStream() = default;
	AssimpStream(const unsigned int severety):severety(severety){}
	~AssimpStream() = default;

	unsigned int severety = 0;

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
};


class ModuleModelLoader : public Module
{
public:
	ModuleModelLoader() = default;
	~ModuleModelLoader() = default;

	bool Init();
	bool CleanUp();

	void SwapCurrentModel(const char *new_model_file_path);
	void SwapCurrentModelTexture(const char *new_texture_file_path);

public:
	const aiScene* scene = nullptr;
	Model* current_model = nullptr;

private:
	void UnloadCurrentModel();
	bool LoadModel(const char *new_model_file_path);

	Mesh* LoadMeshData(const aiMesh *mesh) const;
	Texture* LoadMaterialData(const aiMaterial *material, std::string model_base_path);

	std::string GetModelBasePath(const char *model_file_path) const;
	std::string GetTextureFileName(const char *texture_file_path) const;

};

#endif //_MODULEMODELLOADER_H_