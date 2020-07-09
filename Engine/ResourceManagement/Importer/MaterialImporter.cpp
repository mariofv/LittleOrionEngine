#include "MaterialImporter.h"

#include "Filesystem/PathAtlas.h"
#include "Helper/Config.h"
#include "Main/Application.h"
#include "Module/ModuleFileSystem.h"
#include "Module/ModuleResourceManager.h"

#include "ResourceManagement/Importer/TextureImporter.h"
#include "ResourceManagement/Resources/Material.h"
#include "ResourceManagement/Metafile/TextureMetafile.h"

#include <assimp/scene.h>

#include <regex>


FileData MaterialImporter::ExtractData(Path& assets_file_path, const Metafile& metafile) const
{
	return assets_file_path.GetFile()->Load();
}

FileData MaterialImporter::ExtractMaterialFromAssimp(const aiMaterial* assimp_mesh_material, const Path& material_file_folder_path) const
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

			Metafile* metafile = App->resources->resource_DB->GetEntry(material_texture_uuid);
			if (material_texture_uuid != 0)
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
				case aiTextureType_NORMALS:
					material_config.AddUInt(material_texture_uuid, "Normal");
					break;
				case aiTextureType_LIGHTMAP:
					material_config.AddUInt(material_texture_uuid, "Lightmap");
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

uint32_t MaterialImporter::ImportMaterialTexture(const std::string& texture_described_path, const Path& material_file_folder_path) const
{
	std::string sanitazed_string = texture_described_path;
	size_t pos = 0;
	while ((pos = sanitazed_string.find("\\", pos)) != std::string::npos) {
		sanitazed_string.replace(pos, std::string("\\").length(), "/");
		pos += std::string("/").length();
	}

	RESOURCES_LOG_INFO("Loading material texture in described path %s.", sanitazed_string.c_str());
	if (App->filesystem->Exists(sanitazed_string))
	{
		RESOURCES_LOG_INFO("Material loaded correctly from %s.", sanitazed_string.c_str());
		return App->resources->InternalImport(*App->filesystem->GetPath(sanitazed_string));
	}

	std::string texture_file_name = texture_described_path.substr(sanitazed_string.find_last_of('/') + 1, -1);;
	std::string textures_path = material_file_folder_path.GetFullPath() + "/" + texture_file_name;
	RESOURCES_LOG_INFO("Loading material texture in model folder path %s.", material_file_folder_path.GetFullPath().c_str());
	if (App->filesystem->Exists(textures_path))
	{
		RESOURCES_LOG_INFO("Material loaded correctly from %s.", textures_path.c_str());
		return App->resources->InternalImport(*App->filesystem->GetPath(textures_path));
	}

	textures_path = std::string(TEXTURES_PATH) + "/" +  texture_file_name;
	RESOURCES_LOG_INFO("Loading material texture in textures folder %s.", textures_path.c_str());
	if (App->filesystem->Exists(textures_path))
	{
		RESOURCES_LOG_INFO("Material loaded correctly from %s.", textures_path.c_str());
		return App->resources->InternalImport(*App->filesystem->GetPath(textures_path));
	}

	return 0;
}
