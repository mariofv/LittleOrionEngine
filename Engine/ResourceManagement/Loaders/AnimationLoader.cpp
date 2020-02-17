#include "AnimationLoader.h"


#include "Brofiler/Brofiler.h"
#include <vector>
#include <Main/Application.h>
#include <Module/ModuleFileSystem.h>
#include <ResourceManagement/Resources/Animation.h>

std::shared_ptr<Animation> AnimationLoader::Load(const std::string& uid)
{
	BROFILER_CATEGORY("Load Animation", Profiler::Color::Brown);


	APP_LOG_INFO("Loading Animation %s.", uid.c_str());
	size_t animation_size;
	char * data = App->filesystem->Load(uid.c_str(), animation_size);
	char* cursor = data;


	//Get Animation data
	uint32_t name_size;
	memcpy(&name_size, cursor, sizeof(uint32_t));
	cursor += sizeof(uint32_t);

	std::string animation_name;
	animation_name.resize(name_size);
	memcpy(animation_name.data(), cursor,name_size);
	cursor += name_size;

	float animation_duration;
	memcpy(&animation_duration, cursor, sizeof(float));
	cursor += sizeof(float); // Get duration

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
			memcpy(channel.name.data(), cursor, name_size);
			cursor += name_size;

			memcpy(&channel.position, cursor,sizeof(float4x4));
			cursor += sizeof(float4x4);
		}
	}

	std::shared_ptr<Animation> new_animation = std::make_shared<Animation>(std::move(keyframes), animation_name, animation_duration, uid);
	free(data);

	return new_animation;
}