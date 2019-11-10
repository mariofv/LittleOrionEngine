#pragma once
#include "Module.h"
#include "Globals.h"
#include "Mesh.h"
#include "BoundingBox.h"

#include <GL/glew.h>
#include <vector>
#include <string>

struct aiMesh;
struct aiMaterial;
struct aiScene;

class ModuleModelLoader : public Module
{
public:
	ModuleModelLoader();
	~ModuleModelLoader();

	bool Init();
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();

	void SwapCurrentModel(const char *new_model_file_path);

public:
	std::vector<Mesh*> meshes;
	GLuint *textures = nullptr;
	const aiScene* scene = nullptr;

	BoundingBox *model_bounding_box = nullptr;

private:
	void UnloadCurrentModel();
	bool LoadModel(const char *new_model_file_path);

	void LoadMeshData(const aiMesh *mesh);
	void LoadMaterialData(const aiMaterial *material, const GLuint &texture, std::string model_base_path);

	void ComputeBoundingBox();

	std::string GetModelBasePath(const char *model_file_path) const;
};
