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
	std::shared_ptr<Material> new_material = std::make_shared<Material>(0,file_path);
	new_material->Load(material_config);

	return new_material;
}
