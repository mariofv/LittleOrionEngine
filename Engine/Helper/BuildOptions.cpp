#include "BuildOptions.h"

#include "Filesystem/File.h"
#include "Filesystem/PathAtlas.h"
#include "Helper/Config.h"
#include "Main/Application.h"
#include "Module/ModuleFileSystem.h"

void BuildOptions::AddScene(uint32_t scene_uuid, const std::string& path, const std::string& name)
{
	Options options = Options(scene_uuid, path, name);
	build_scenes.push_back(options);
}

void BuildOptions::RemoveScene(unsigned position)
{
	build_scenes.erase(build_scenes.begin() + position);
}

bool BuildOptions::AnySceneLoadable() const
{
	return build_mode_activated && !build_scenes.empty();
}

void BuildOptions::SetBuildModeActive()
{
	build_mode_activated = !build_mode_activated;
}

bool BuildOptions::LoadOptions()
{

	if(!App->filesystem->Exists(RESOURCES_BUILD_OPTIONS))
	{
		return false;
	}

	Path* build_options_file_path = App->filesystem->GetPath(RESOURCES_BUILD_OPTIONS);

	FileData build_options_data = build_options_file_path->GetFile()->Load();
	size_t readed_bytes = build_options_data.size;
	char* scene_file_data = (char*)build_options_data.buffer;

	if (scene_file_data == nullptr)
	{
		APP_LOG_INFO("No data founded when loading building options.")
		return false;
	}


	std::string serialized_scene_string = scene_file_data;
	free(scene_file_data);

	Config build_config(serialized_scene_string);
	std::vector<Config> build_options_configs;

	build_config.GetChildrenConfig("BuildOptions", build_options_configs);
	for (unsigned int i = 0; i < build_options_configs.size(); ++i)
	{
		Options option;
		option.Load(build_options_configs[i]);

		build_scenes.push_back(option);
	}

	return true;
}

bool BuildOptions::SaveOptions() const
{
	if (build_scenes.empty())
	{
		APP_LOG_INFO("Options are empty, cannot save build options.")
		return false;
	}

	Config options_config;
	std::vector<Config> options_configs_vector;
	for(auto& option : build_scenes)
	{
		Config option_config;
		option.Save(option_config);
		options_configs_vector.push_back(option_config);
	}

	options_config.AddChildrenConfig(options_configs_vector, "BuildOptions");

	std::string serialized_build_options_string;
	options_config.GetSerializedString(serialized_build_options_string);

	App->filesystem->Save(RESOURCES_BUILD_OPTIONS, serialized_build_options_string);

	return true;
}

uint32_t BuildOptions::GetSceneUUID(unsigned position) const
{
	assert(position < build_scenes.size());
	return build_scenes[position].uuid;
}

std::string BuildOptions::GetScenePath(unsigned position) const
{
	assert(position < build_scenes.size());
	return build_scenes[position].library_path;
}
