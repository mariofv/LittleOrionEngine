#include "MaterialImporter.h"

#include "Helper/Config.h"
#include "Main/Application.h"
#include "Module/ModuleFileSystem.h"
#include "Module/ModuleResourceManager.h"

#include "ResourceManagement/Importer/TextureImporter.h"
#include "ResourceManagement/Resources/Material.h"

#include <assimp/scene.h>

ImportResult MaterialImporter::Import(const File& file, bool force) const
{
	ImportResult import_result;

	ImportOptions already_imported = GetAlreadyImportedResource(file);
	if (already_imported.uuid != 0 && !force) {
		APP_LOG_INFO("Material already imported.")
		import_result.success = true;
		import_result.exported_file = already_imported.exported_file;
		return import_result;
	}

	std::string library_material_file = SaveMetaFile(file.file_path, ResourceType::MATERIAL);

	App->filesystem->Copy(file.file_path.c_str(), library_material_file.c_str());
	

	import_result.success = true;
	import_result.exported_file = library_material_file;
	return import_result;
}


ImportResult MaterialImporter::ExtractMaterialFromMesh(const aiScene* scene, size_t mesh_index, const char* model_file_path, const char* material_assets_file_path) const
{
	if (App->filesystem->Exists(material_assets_file_path))
	{
		APP_LOG_INFO("Material %s already exists.", material_assets_file_path)
		return ImportResult();
	}

	Material imported_material(0, std::string(material_assets_file_path));

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

	material_config.AddBool(imported_material.show_checkerboard_texture, "Checkboard");
	material_config.AddString(imported_material.shader_program, "ShaderProgram");

	//k
	material_config.AddFloat(imported_material.k_ambient, "kAmbient");
	material_config.AddFloat(imported_material.k_specular, "kSpecular");
	material_config.AddFloat(imported_material.k_diffuse, "kDiffuse");

	//colors
	material_config.AddColor(
		float4(
			imported_material.diffuse_color[0],
			imported_material.diffuse_color[1],
			imported_material.diffuse_color[2],
			imported_material.diffuse_color[3]
		),
		"difusseColor");

	material_config.AddColor(
		float4(
			imported_material.emissive_color[0],
			imported_material.emissive_color[1],
			imported_material.emissive_color[2],
			1.0f
		),
		"emissiveColor"
	);

	material_config.AddColor(
		float4(
			imported_material.specular_color[0],
			imported_material.specular_color[1],
			imported_material.specular_color[2],
			1.0f
		),
		"specularColor"
	);

	std::string serialized_material_string;
	material_config.GetSerializedString(serialized_material_string);

	App->filesystem->Save(material_assets_file_path, serialized_material_string.c_str(), serialized_material_string.size() + 1);
	return Import(File(material_assets_file_path));
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