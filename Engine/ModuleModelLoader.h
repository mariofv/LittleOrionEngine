#pragma once
#include "Module.h"
#include "Globals.h"
#include "Mesh.h"
#include <GL/glew.h>
#include <vector>

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

public:
	std::vector<Mesh*> meshes;
	/*
	GLuint *vaos = nullptr;
	GLuint *vbos = nullptr;
	GLuint *ibos = nullptr;
	*/
	GLuint *textures = nullptr;
	const aiScene* scene = nullptr;

private:
	void LoadMeshData(const aiMesh *mesh);
	void LoadMaterialData(const aiMaterial *material, const GLuint &texture);
};
