#include "MaterialLoader.h"

#include "Helper/Config.h"
#include "Main/Application.h"
#include "Module/ModuleFileSystem.h"
#include "Module/ModuleResourceManager.h"
#include "ResourceManagement/Resources/Material.h"
#include "ResourceManagement/Resources/Texture.h"

#include <Brofiler/Brofiler.h>
#include <vector>

std::shared_ptr<Material> MaterialLoader::Load(const std::string& file_path)
{
	BROFILER_CATEGORY("Load Material", Profiler::Color::Brown);


	APP_LOG_INFO("Loading Material %s.", file_path.c_str());

	size_t readed_bytes;
	char* material_file_data = App->filesystem->Load(file_path.c_str(), readed_bytes);

	if (material_file_data == nullptr)
	{
		APP_LOG_ERROR("Error loading Material %s.", file_path.c_str());
		return nullptr;
	}

	std::string serialized_material_string = material_file_data;
	free(material_file_data);

	Config material_config(serialized_material_string);
	std::shared_ptr<Material> new_material = std::make_shared<Material>(file_path);

	std::string texture_path;
	material_config.GetString("Diffuse", texture_path, "");
	std::shared_ptr<Texture> texture_resource = App->resources->Load<Texture>(texture_path);
	if (texture_resource.get() != nullptr)
	{
		new_material.get()->SetMaterialTexture(Texture::TextureType::DIFUSSE, texture_resource);
	}

	material_config.GetString("Specular", texture_path, "");
	texture_resource = App->resources->Load<Texture>(texture_path);
	if (texture_resource.get() != nullptr)
	{
		new_material.get()->SetMaterialTexture(Texture::TextureType::SPECULAR, texture_resource);
	}

	material_config.GetString("Occlusion", texture_path, "");
	texture_resource = App->resources->Load<Texture>(texture_path);
	if (texture_resource.get() != nullptr)
	{
		new_material.get()->SetMaterialTexture(Texture::TextureType::OCLUSION, texture_resource);
	}

	material_config.GetString("Emissive", texture_path, "");
	texture_resource = App->resources->Load<Texture>(texture_path);
	if (texture_resource.get() != nullptr)
	{
		new_material.get()->SetMaterialTexture(Texture::TextureType::EMISSIVE, texture_resource);
	}

	return new_material;
}
