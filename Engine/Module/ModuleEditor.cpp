#include "Globals.h"
#include "Application.h"
#include "ModuleEditor.h"
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

update_status ModuleEditor::PreUpdate()
{
	return update_status::UPDATE_CONTINUE;
}

// Called every draw update
update_status ModuleEditor::Update()
{
	return update_status::UPDATE_CONTINUE;
}

update_status ModuleEditor::PostUpdate()
{
	return update_status::UPDATE_CONTINUE;
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
	std::ifstream ifs(path);
	std::string serialized_scene_string(
		(std::istreambuf_iterator<char>(ifs)),
		(std::istreambuf_iterator<char>())
	);

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

	std::ofstream scene_file;
	scene_file.open(path);
	scene_file << serialized_scene_string;
	scene_file.close();
}

void ModuleEditor::LoadSceneTmp() const
{
	std::ifstream ifs(TMP_SCENE_PATH);
	std::string serialized_scene_string(
		(std::istreambuf_iterator<char>(ifs)),
		(std::istreambuf_iterator<char>())
	);

	Config scene_config(serialized_scene_string);
	App->scene->Load(scene_config);

	remove(TMP_SCENE_PATH);
}

void ModuleEditor::SaveSceneTmp() const
{
	Config scene_config;
	App->scene->Save(scene_config);
	std::string serialized_scene_string;
	scene_config.GetSerializedString(serialized_scene_string);

	std::ofstream scene_file;
	scene_file.open(TMP_SCENE_PATH);
	scene_file << serialized_scene_string;
	scene_file.close();
}