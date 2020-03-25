#include "AnimationManager.h"

#include "Main/Application.h"
#include "Module/ModuleResourceManager.h"
#include "Module/ModuleFileSystem.h"
#include "ResourceManagement/Metafile/Metafile.h"
#include "ResourceManagement/Resources/Animation.h"

#include <Brofiler/Brofiler.h>
#include <vector>

std::shared_ptr<Animation> AnimationManager::Load(Metafile* metafile, const FileData& resource_data)
{
	char* cursor = (char*)resource_data.buffer;

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

			memcpy(&channel.position, cursor,sizeof(float4x4));
			cursor += sizeof(float4x4);
		}
	}

	std::shared_ptr<Animation> new_animation = std::make_shared<Animation>(metafile, std::move(keyframes), animation_name, animation_frames, frames_per_second);

	return new_animation;
}
