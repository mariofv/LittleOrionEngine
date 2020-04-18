#include "SceneManager.h"

#include "Helper/Config.h"
#include "Main/Application.h"
#include "Module/ModuleFileSystem.h"
#include "Module/ModuleScene.h"
#include "Module/ModuleResourceManager.h"
#include "ResourceManagement/Metafile/Metafile.h"
#include "ResourceManagement/Resources/Scene.h"

std::shared_ptr<Scene> SceneManager::Load(Metafile* metafile, const FileData& resource_data)
{
	char* scene_file_data = (char*)resource_data.buffer;
	std::string serialized_scene_string = std::string(scene_file_data, resource_data.size);

	Config scene_config(serialized_scene_string);
	std::shared_ptr<Scene> new_scene = std::make_shared<Scene>(metafile, scene_config);
	new_scene->Load();

	return new_scene;
}

uint32_t SceneManager::Create(const std::string& new_scene_path)
{
	Scene scene;
	scene.Save(App->scene->GetRoot());
	std::string serialized_scene_string = scene.GetSerializedConfig();

	char* scene_bytes = new char[serialized_scene_string.size() + 1];
	memcpy(scene_bytes, serialized_scene_string.c_str(), serialized_scene_string.size() + 1);

	FileData scene_data{ scene_bytes, serialized_scene_string.size() + 1 };

	return App->resources->CreateFromData(scene_data, new_scene_path);
}
