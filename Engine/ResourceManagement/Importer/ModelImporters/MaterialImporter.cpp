#include "MaterialImporter.h"

#include "Helper/Config.h"
#include "Main/Application.h"
#include "Module/ModuleFileSystem.h"
#include "Module/ModuleResourceManager.h"
#include "ResourceManagement/Importer/TextureImporter.h"

#include <assimp/scene.h>

void MaterialImporter::ImportTexturesFromMesh(const aiScene* scene, size_t mesh_index, const char* file_path, std::vector<std::string> & loaded_meshes_materials) const
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

void MaterialImporter::ImportMaterialFromMesh(const aiScene* scene, size_t mesh_index, const char* model_file_path, const char* material_file_path) const
{
	int mesh_material_index = scene->mMeshes[mesh_index]->mMaterialIndex;
	std::string model_base_path = std::string(model_file_path);
	
	Config material_config;
	aiTextureMapping mapping = aiTextureMapping_UV;
	for (size_t i = 0; i < AI_TEXTURE_TYPE_MAX; i++)
	{
		aiTextureType type = static_cast<aiTextureType>(i);
		for (size_t j = 0; j < scene->mMaterials[mesh_material_index]->GetTextureCount(type); j++)
		{
			aiString file;
			scene->mMaterials[mesh_material_index]->GetTexture(type, j, &file, &mapping, 0);
			std::string material_texture = App->resources->texture_importer->ImportMaterialData(file.data, model_base_path);

			if (!material_texture.empty())
			{
				switch (type)
				{
				case aiTextureType_DIFFUSE:
					material_config.AddString(material_texture, "Diffuse");
					break;
				case aiTextureType_SPECULAR:
					material_config.AddString(material_texture, "Specular");
					break;
				case aiTextureType_EMISSIVE:
					material_config.AddString(material_texture, "Emissive");
					break;
				case aiTextureType_AMBIENT_OCCLUSION:
					material_config.AddString(material_texture, "Occlusion");
					break;
				default:
					material_config.AddString(material_texture, "Unknown");
					break;
				}
			}
		}
	}
	
	std::string serialized_scene_string;
	material_config.GetSerializedString(serialized_scene_string);

	App->filesystem->Save(material_file_path, serialized_scene_string.c_str(), serialized_scene_string.size() + 1);
}

Material::MaterialTextureType MaterialImporter::GetTextureTypeFromAssimpType(aiTextureType type) const
{
	switch (type)
	{
	case aiTextureType_DIFFUSE:
		return Material::MaterialTextureType::DIFFUSE;
		break;
	case aiTextureType_SPECULAR:
		return Material::MaterialTextureType::SPECULAR;
		break;
	case aiTextureType_EMISSIVE:
		return Material::MaterialTextureType::EMISSIVE;
		break;
	case aiTextureType_AMBIENT_OCCLUSION:
		return Material::MaterialTextureType::OCCLUSION;
		break;
	default:
		return Material::MaterialTextureType::UNKNOWN;
		break;
	}
}