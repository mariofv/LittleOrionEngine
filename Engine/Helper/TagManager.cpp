#include "TagManager.h"

#include "Filesystem/File.h"
#include "Filesystem/PathAtlas.h"
#include "Helper/Config.h"
#include "Log/EngineLog.h"

#include "Main/Application.h"
#include "Module/ModuleFileSystem.h"
#include "Module/ModuleScene.h"


void TagManager::AddTag(const std::string& new_tag)
{
	if (std::find(tags.begin(), tags.end(), new_tag) != tags.end())
	{
		APP_LOG_INFO("Tag %s already exists", new_tag.c_str());
		return;
	}
		
	tags.push_back(new_tag);
	SaveTags();
}

void TagManager::RemoveTag(const std::string& tag_to_remove)
{
	auto tag_position = std::find(tags.begin(), tags.end(), tag_to_remove);
	if (tag_position == tags.end())
	{
		APP_LOG_INFO("Tag %s doesn't exists.", tag_to_remove.c_str());
		return;
	}

	std::vector<GameObject*> tagged_game_objects = App->scene->GetGameObjectsWithTag(tag_to_remove);
	for (auto& tagged_game_object : tagged_game_objects)
	{
		tagged_game_object->tag = "";
	}
	tags.erase(tag_position);

	SaveTags();
}

bool TagManager::LoadTags()
{
	if (!App->filesystem->Exists(PROJECT_TAGS_PATH))
	{
		APP_LOG_INFO("Tags file \"%s\" doesn't exist.", PROJECT_TAGS_PATH)
		return false;
	}

	Path* tags_file_path = App->filesystem->GetPath(PROJECT_TAGS_PATH);

	FileData tags_file_data = tags_file_path->GetFile()->Load();
	size_t readed_bytes = tags_file_data.size;
	char* tags_data = (char*)tags_file_data.buffer;

	if (tags_data == nullptr)
	{
		APP_LOG_INFO("No data founded when loading tags.")
		return false;
	}

	std::string serialized_tags_string = tags_data;
	delete[] tags_data;

	Config tags_config(serialized_tags_string);
	tags_config.GetVector<std::string>("Tags", tags, {});

	return true;
}

bool TagManager::SaveTags() const
{
	Config tags_config;
	tags_config.AddVector<std::string>(tags, "Tags");

	std::string serialized_tags_string;
	tags_config.GetSerializedString(serialized_tags_string);

	if (App->filesystem->Exists(PROJECT_TAGS_PATH))
	{
		App->filesystem->Save(PROJECT_TAGS_PATH, serialized_tags_string);
	}
	else
	{
		APP_LOG_ERROR("Couldn't update tags file, please create it under the path %s", PROJECT_TAGS_PATH);
		return false;
	}

	return true;
}