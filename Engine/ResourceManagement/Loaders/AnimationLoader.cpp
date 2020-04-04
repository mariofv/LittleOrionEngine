#include "AnimationLoader.h"


#include "Brofiler/Brofiler.h"
#include <vector>
#include <Main/Application.h>
#include <Module/ModuleFileSystem.h>
#include <ResourceManagement/Resources/Animation.h>

std::shared_ptr<Animation> AnimationLoader::Load(const std::string& uuid)
{
	BROFILER_CATEGORY("Load Animation", Profiler::Color::Brown);


	if (!App->filesystem->Exists(uuid.c_str()))
	{
		APP_LOG_ERROR("Error loading Animation %s.", uuid.c_str());
		return nullptr;
	}
	APP_LOG_INFO("Loading Animation %s.", uuid.c_str());
	size_t animation_size;
	char * data = App->filesystem->Load(uuid.c_str(), animation_size);
	char* cursor = data;


	//Get Animation data
	uint32_t name_size;
	memcpy(&name_size, cursor, sizeof(uint32_t));
	cursor += sizeof(uint32_t);

	std::string animation_name;
	animation_name.resize(name_size);
	memcpy(&animation_name[0], cursor,name_size);
	cursor += name_size;

	float animation_frames;
	memcpy(&animation_frames, cursor, sizeof(float));
	cursor += sizeof(float); // Get duration

	float frames_per_second;
	memcpy(&frames_per_second, cursor, sizeof(float));
	cursor += sizeof(float); // Get frames_per_second

	uint32_t num_keyframe;
	memcpy(&num_keyframe, cursor, sizeof(uint32_t));
	cursor += sizeof(uint32_t); // Get keyframes

	std::vector<Animation::KeyFrame> keyframes;
	keyframes.resize(num_keyframe);
	for (auto & keyframe : keyframes)
	{
		memcpy(&keyframe.frame, cursor,sizeof(float));
		cursor += sizeof(float);

		uint32_t number_channels = keyframe.channels.size();
		memcpy(&number_channels,cursor,sizeof(uint32_t));
		cursor += sizeof(uint32_t);

		keyframe.channels.resize(number_channels);


		for (auto & channel : keyframe.channels)
		{
			uint32_t name_size;
			memcpy(&name_size, cursor, sizeof(uint32_t));
			cursor += sizeof(uint32_t);

			channel.name.resize(name_size);
			memcpy(&channel.name[0], cursor, name_size);
			cursor += name_size;

			memcpy(&channel.is_translated, cursor, sizeof(bool));
			cursor += sizeof(bool);

			memcpy(&channel.translation, cursor,sizeof(float3));
			cursor += sizeof(float3);

			memcpy(&channel.is_rotated, cursor, sizeof(bool));
			cursor += sizeof(bool);

			memcpy(&channel.rotation, cursor, sizeof(Quat));
			cursor += sizeof(Quat);
		}
	}

	std::string uid_string = uuid.substr(uuid.find_last_of("/") + 1, uuid.size());
	uint32_t real_uuid = std::stoul(uid_string);

	std::shared_ptr<Animation> new_animation = std::make_shared<Animation>(std::move(keyframes), animation_name, animation_frames, frames_per_second,real_uuid,uuid);
	free(data);

	return new_animation;
}