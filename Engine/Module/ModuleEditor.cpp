#include "Globals.h"
#include "Application.h"
#include "ModuleEditor.h"
#include "ModuleFileSystem.h"
#include "ModuleScene.h"
#include "Config.h"

#include <iostream>
#include <fstream>

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

void ModuleEditor::OpenScene() const
{
	const char* path = "./Assets/Scenes/scene.scene";
	size_t readed_bytes;
	std::string serialized_scene_string = App->filesystem->Load(path, readed_bytes);
	
	Config scene_config(serialized_scene_string);
	App->scene->Load(scene_config);
}

void ModuleEditor::SaveScene() const
{
	const char* path = "./Assets/Scenes/scene.scene";

	Config scene_config;
	App->scene->Save(scene_config);
	std::string serialized_scene_string;
	scene_config.GetSerializedString(serialized_scene_string);

	App->filesystem->Save(path, serialized_scene_string.c_str(), serialized_scene_string.size() + 1);
}

void ModuleEditor::OpenDefaultScene() const
{
	size_t readed_bytes;
	std::string serialized_scene_string = App->filesystem->Load(DEFAULT_SCENE_PATH, readed_bytes);

	Config scene_config(serialized_scene_string);
	App->scene->Load(scene_config);
}

void ModuleEditor::OpenTmpScene() const
{
	size_t readed_bytes;
	std::string serialized_scene_string = App->filesystem->Load(TMP_SCENE_PATH, readed_bytes);

	Config scene_config(serialized_scene_string);
	App->scene->Load(scene_config);

	remove(TMP_SCENE_PATH);
}

void ModuleEditor::SaveTmpScene() const
{
	Config scene_config;
	App->scene->Save(scene_config);
	std::string serialized_scene_string;
	scene_config.GetSerializedString(serialized_scene_string);

	App->filesystem->Save(TMP_SCENE_PATH, serialized_scene_string.c_str(), serialized_scene_string.size() + 1);
}