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
	return true;
}

void ModuleEditor::OpenScene() const
{
}

void ModuleEditor::SaveScene() const
{
	const char* path = "./scene.scene";

	SerializedScene serialized_scene;
	App->scene->Save(serialized_scene);
	std::string serialized_scene_string;
	serialized_scene.GetString(serialized_scene_string);

	std::ofstream scene_file;
	scene_file.open(path);
	scene_file << serialized_scene_string;
	scene_file.close();
}