#ifndef _MATERIALIMPORTER_H_
#define _MATERIALIMPORTER_H_

#include "ResourceManagement/Importer/Importer.h"
#include "ResourceManagement/Resources/Material.h"

#include <assimp/material.h>
#include <vector>

struct aiScene;

class MaterialImporter : public Importer
{
public:
	MaterialImporter() = default;
	~MaterialImporter() = default;
	ImportResult Import(const File& file, bool force = false) const override;

	ImportResult ExtractMaterialFromMesh(const aiScene* scene, size_t mesh_index, const char* model_file_path, const char* material_assets_file_path) const;

	Material::MaterialTextureType GetTextureTypeFromAssimpType(aiTextureType type) const;
};

#endif // !_MATERIALIMPORTER_H_
