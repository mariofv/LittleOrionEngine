#include "SerializedScene.h"

#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>

SerializedScene::SerializedScene() 
{
	scene_document.SetObject();
}

SerializedScene::~SerializedScene()
{

}

void SerializedScene::AddGameObjectsConfig(std::vector<Config*>& game_objects_config)
{
	rapidjson::Value game_objects_config_array(rapidjson::kArrayType);
	for (auto& game_object_config : game_objects_config)
	{
		game_objects_config_array.PushBack(game_object_config->config_value.Move(), scene_document.GetAllocator());
		delete game_object_config;
	}
	scene_document.AddMember("GameObjects", game_objects_config_array, scene_document.GetAllocator());
}

void SerializedScene::GetString(std::string& return_string)
{
	rapidjson::StringBuffer buffer;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);

	scene_document.Accept(writer);
	return_string = buffer.GetString();
}

rapidjson::Document::AllocatorType& SerializedScene::GetSceneAllocator()
{
	return scene_document.GetAllocator();
}