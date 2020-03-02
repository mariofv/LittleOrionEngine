#ifndef _MATERIALIMPORTER_H_
#define _MATERIALIMPORTER_H_

#include <vector>
#include <ResourceManagement/Resources/Texture.h>
#include <assimp/material.h>
struct aiScene;
class MaterialImporter
{
public:
	MaterialImporter() = default;
	~MaterialImporter() = default;
	void ImportTexturesFromMesh(const aiScene* scene, size_t mesh_index, const char* file_path, std::vector<std::string> & loaded_meshes_materials) const;
	void ImportMaterialFromMesh(const aiScene* scene, size_t mesh_index, const char* file_path) const;
	Texture::TextureType GetTextureTypeFromAssimpType(aiTextureType type) const;
};

#endif // !_MATERIALIMPORTER_H_
