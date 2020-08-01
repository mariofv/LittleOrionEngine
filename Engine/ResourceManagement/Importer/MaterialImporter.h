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
	MaterialImporter() : Importer(ResourceType::MATERIAL) {};
	~MaterialImporter() = default;
	FileData ExtractMaterialFromAssimp(const aiMaterial* assimp_mesh_material, const Path& material_file_folder_path) const;

private:
	uint32_t ImportMaterialTexture(const std::string& texture_file_name, const Path& material_file_folder_path) const;
};

#endif // !_MATERIALIMPORTER_H_
