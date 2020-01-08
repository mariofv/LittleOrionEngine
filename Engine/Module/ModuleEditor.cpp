#include "Globals.h"
#include "Application.h"
#include "ModuleEditor.h"
#include "ModuleFileSystem.h"
#include "ModuleScene.h"
#include "Config.h"

// Called before render is available
bool ModuleEditor::Init()
{
	APP_LOG_SECTION("************ Module Editor Init ************");
	
	
	APP_LOG_SUCCESS("IMGUI editor initialized correctly.");

	return true;
}

// Called before quitting
bool ModuleEditor::CleanUp()
{
	remove(TMP_SCENE_PATH);

	return true;
}

void ModuleEditor::OpenScene(const std::string &path) const
{
	size_t readed_bytes;
	std::string serialized_scene_string = App->filesystem->Load(path.c_str(), readed_bytes);
	
	Config scene_config(serialized_scene_string);
	App->scene->Load(scene_config);
}

void ModuleEditor::SaveScene(const std::string &path) const
{
	Config scene_config;
	App->scene->Save(scene_config);
	std::string serialized_scene_string;
	scene_config.GetSerializedString(serialized_scene_string);

	App->filesystem->Save(path.c_str(), serialized_scene_string.c_str(), serialized_scene_string.size() + 1);
}