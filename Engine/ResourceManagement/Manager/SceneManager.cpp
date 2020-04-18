#include "SceneManager.h"

#include "Helper/Config.h"
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
