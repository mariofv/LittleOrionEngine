#include "SceneManager.h"

#include "Helper/Config.h"
#include "Main/Application.h"
#include "Module/ModuleFileSystem.h"
#include "Module/ModuleScene.h"
#include "Module/ModuleResourceManager.h"
#include "ResourceManagement/Metafile/Metafile.h"
#include "ResourceManagement/Resources/Scene.h"

FileData SceneManager::Binarize(Scene* scene)
{
	scene->Save(App->scene->GetRoot());
	std::string serialized_scene_string = scene->GetSerializedConfig();

	char* scene_bytes = new char[serialized_scene_string.size() + 1];
	memcpy(scene_bytes, serialized_scene_string.c_str(), serialized_scene_string.size() + 1);

	return FileData{scene_bytes, serialized_scene_string.size() + 1 }; 
}

std::shared_ptr<Scene> SceneManager::Load(uint32_t uuid, const FileData& resource_data)
{
	char* scene_file_data = (char*)resource_data.buffer;
	std::string serialized_scene_string = std::string(scene_file_data, resource_data.size);

	Config scene_config(serialized_scene_string);
	return std::make_shared<Scene>(uuid, scene_config);
}

uint32_t SceneManager::Create(const std::string& new_scene_path)
{
	Scene scene;
	return App->resources->CreateFromData(Binarize(&scene) , new_scene_path);
}
