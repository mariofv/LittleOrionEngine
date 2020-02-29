#include "MaterialLoader.h"

#include "Main/Application.h"
#include "Module/ModuleFileSystem.h"
#include "ResourceManagement/Resources/Material.h"

#include <Brofiler/Brofiler.h>
#include <vector>

std::shared_ptr<Material> MaterialLoader::Load(const std::string& file_path)
{
	BROFILER_CATEGORY("Load Material", Profiler::Color::Brown);


	APP_LOG_INFO("Loading Material %s.", file_path.c_str());


	return nullptr;
}
