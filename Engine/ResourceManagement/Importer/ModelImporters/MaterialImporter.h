#ifndef _MATERIALIMPORTER_H_
#define _MATERIALIMPORTER_H_

#include <vector>
#include <ResourceManagement/Resources/Material.h>
#include <assimp/material.h>
struct aiScene;
class MaterialImporter
{
public:
	MaterialImporter() = default;
	~MaterialImporter() = default;

	void ImportMaterialFromMesh(
		const aiScene* scene,
		size_t mesh_index, 
		const char* model_file_path, 
		const char* material_assets_file_path, 
		const char* material_library_file_path
	) const;

	Material::MaterialTextureType GetTextureTypeFromAssimpType(aiTextureType type) const;
};

#endif // !_MATERIALIMPORTER_H_
