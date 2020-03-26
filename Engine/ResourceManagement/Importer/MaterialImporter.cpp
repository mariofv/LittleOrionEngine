#include "MaterialImporter.h"

#include "Filesystem/PathAtlas.h"
#include "Helper/Config.h"
#include "Main/Application.h"
#include "Module/ModuleFileSystem.h"
#include "Module/ModuleResourceManager.h"

#include "ResourceManagement/Importer/TextureImporter.h"
#include "ResourceManagement/Resources/Material.h"

#include <assimp/scene.h>

FileData MaterialImporter::ExtractData(Path& assets_file_path) const 
{
	return assets_file_path.GetFile()->Load();
}


FileData MaterialImporter::ExtractMaterialFromAssimp(const aiMaterial* assimp_mesh_material, const Path& material_file_folder_path)
{
	Material imported_material;

	Config material_config;
	aiTextureMapping mapping = aiTextureMapping_UV;
	for (size_t i = 0; i < AI_TEXTURE_TYPE_MAX; i++)
	{
		aiTextureType type = static_cast<aiTextureType>(i);
		for (size_t j = 0; j < assimp_mesh_material->GetTextureCount(type); j++)
		{
			aiString file;
			assimp_mesh_material->GetTexture(type, j, &file, &mapping, 0);
			uint32_t material_texture_uuid = ImportMaterialTexture(file.data, material_file_folder_path);

			if (material_texture_uuid != -1)
			{
				switch (type)
				{
				case aiTextureType_DIFFUSE:
					material_config.AddUInt(material_texture_uuid, "Diffuse");
					break;
				case aiTextureType_SPECULAR:
					material_config.AddUInt(material_texture_uuid, "Specular");
					break;
				case aiTextureType_EMISSIVE:
					material_config.AddUInt(material_texture_uuid, "Emissive");
					break;
				case aiTextureType_AMBIENT_OCCLUSION:
					material_config.AddUInt(material_texture_uuid, "Occlusion");
					break;
				default:
					material_config.AddUInt(material_texture_uuid, "Unknown");
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
	material_config.AddFloat(imported_material.shininess, "shininess");

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

	char* material_bytes = new char[serialized_material_string.size() + 1];
	memcpy(material_bytes, serialized_material_string.c_str(), serialized_material_string.size() + 1);

	FileData material_data{ material_bytes, serialized_material_string.size() + 1 };
	return material_data;
}

uint32_t MaterialImporter::ImportMaterialTexture(const std::string& texture_described_path, const Path& material_file_folder_path)
{
	APP_LOG_INIT("Loading material texture in described path %s.", texture_described_path.c_str());
	if (App->filesystem->Exists(texture_described_path))
	{
		APP_LOG_SUCCESS("Material loaded correctly from %s.", texture_described_path.c_str());
		return App->resources->Import(*App->filesystem->GetPath(texture_described_path));
	}

	std::string texture_file_name = texture_described_path.substr(texture_described_path.find_last_of('/') + 1, -1);;
	std::string textures_path = material_file_folder_path.GetFullPath() + "/" + texture_file_name;
	APP_LOG_INIT("Loading material texture in model folder path %s.", material_file_folder_path.GetFullPath().c_str());
	if (App->filesystem->Exists(textures_path))
	{
		APP_LOG_SUCCESS("Material loaded correctly from %s.", textures_path.c_str());
		return App->resources->Import(*App->filesystem->GetPath(textures_path));
	}

	textures_path = std::string(TEXTURES_PATH) + "/" +  texture_file_name;
	APP_LOG_INIT("Loading material texture in textures folder %s.", textures_path.c_str());
	if (App->filesystem->Exists(textures_path))
	{
		APP_LOG_SUCCESS("Material loaded correctly from %s.", textures_path.c_str());
		return App->resources->Import(*App->filesystem->GetPath(textures_path));
	}

	return -1;
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