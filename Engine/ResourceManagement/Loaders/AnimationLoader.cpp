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

	uint32_t num_channels;
	memcpy(&num_channels, cursor, sizeof(uint32_t));
	cursor += sizeof(uint32_t); // Get channels

	std::vector<Animation::Channel> channels;
	channels.resize(num_channels);
	for (auto & channel : channels)
	{

		uint32_t name_size;
		memcpy(&name_size, cursor, sizeof(uint32_t));
		cursor += sizeof(uint32_t);

		channel.name.resize(name_size);
		memcpy(channel.name.data(), cursor, name_size);
		cursor += name_size;

		uint32_t positions_size;
		memcpy(&positions_size, cursor, sizeof(uint32_t));
		cursor += sizeof(uint32_t);

		memcpy(channel.positions.data(), cursor, sizeof(Animation::JointPosition) * positions_size);
		cursor += sizeof(Animation::JointPosition) * positions_size;

	}
	std::shared_ptr<Animation> new_animation = std::make_shared<Animation>(std::move(channels), animation_name, animation_duration, uid);
	free(data);

	return new_animation;
}