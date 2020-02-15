#include "MaterialImporter.h"
#include <assimp/scene.h>
#include "Main/Application.h"
#include "Module/ModuleResourceManager.h"
#include <ResourceManagement/Importer/TextureImporter.h>
void MaterialImporter::ImportMaterialFromMesh(const aiScene* scene, size_t mesh_index, const char* file_path, std::vector<std::string> & loaded_meshes_materials) const
{
	int mesh_material_index = scene->mMeshes[mesh_index]->mMaterialIndex;
	std::string model_base_path = std::string(file_path);
	aiTextureMapping mapping = aiTextureMapping_UV;
	for (size_t i = 0; i < AI_TEXTURE_TYPE_MAX; i++)
	{
		aiTextureType type = static_cast<aiTextureType>(i);
		for (size_t j = 0; j < scene->mMaterials[mesh_material_index]->GetTextureCount(type); j++)
		{
			aiString file;
			scene->mMaterials[mesh_material_index]->GetTexture(type, j, &file, &mapping, 0);
			std::string material_texture = App->resources->texture_importer->ImportMaterialData(file.data, model_base_path);
			int texture_type = static_cast<int>(GetTextureTypeFromAssimpType(type));
			material_texture = std::to_string(texture_type) + ":" + material_texture;
			if (!material_texture.empty())
			{
				loaded_meshes_materials.push_back(material_texture);
			}
		}
	}
}


Texture::TextureType MaterialImporter::GetTextureTypeFromAssimpType(aiTextureType type) const
{
	switch (type)
	{
	case aiTextureType_DIFFUSE:
		return Texture::TextureType::DIFUSSE;
		break;
	case aiTextureType_SPECULAR:
		return Texture::TextureType::SPECULAR;
		break;
	case aiTextureType_EMISSIVE:
		return Texture::TextureType::EMISSIVE;
		break;
	case aiTextureType_AMBIENT_OCCLUSION:
		return Texture::TextureType::OCLUSION;
		break;
	default:
		return Texture::TextureType::UNKNOWN;
		break;
	}
}