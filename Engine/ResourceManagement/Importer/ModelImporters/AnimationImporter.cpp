#include "AnimationImporter.h"

#include "Helper/Utils.h"
#include "Main/Application.h"
#include "Module/ModuleFileSystem.h"
#include "ResourceManagement/Resources/Animation.h"

#include <assimp/scene.h>
#include <map>

bool AnimationImporter::ImportAnimation(const aiScene* scene, const aiAnimation* animation, std::string& output_file) const
{
	float unit_scale_factor = 1.f;
	for (unsigned int i = 0; i < scene->mMetaData->mNumProperties; ++i)
	{
		if (scene->mMetaData->mKeys[i] == aiString("UnitScaleFactor"))
		{
			aiMetadataEntry unit_scale_entry = scene->mMetaData->mValues[i];
			unit_scale_factor = *(double*)unit_scale_entry.mData;
		};
	}
	unit_scale_factor *= 0.01f;

	Animation own_format_animation("", "");
	GetCleanAnimation(animation, own_format_animation, unit_scale_factor);
	own_format_animation.duration = static_cast<float>(animation->mDuration);
	own_format_animation.name = std::string(animation->mName.C_Str());

	App->filesystem->MakeDirectory(LIBRARY_ANIMATION_FOLDER);
	output_file = LIBRARY_ANIMATION_FOLDER + "/" + std::string(scene->mRootNode->mName.C_Str()) + "_" + own_format_animation.name + ".anim";

	SaveBinary(own_format_animation, output_file);
	return true;
}

/*
A channel is all the positions for a joint in each frame.
aiNodeAnim contains the position in each frame.

Assimp added nodes need to be merge with the real node.

*/

void AnimationImporter::GetCleanAnimation(const aiAnimation* animation, Animation& own_format_animation, float scale_factor) const
{
	std::unordered_map<std::string, std::vector<aiNodeAnim *>> aiNode_by_channel;


	//Organize channels
	for (size_t i = 0; i < animation->mNumChannels; i++)
	{
		aiNodeAnim* channel = animation->mChannels[i];
		std::string channel_name(channel->mNodeName.C_Str());

		size_t assimp_key_index = channel_name.find("$Assimp");
		if (assimp_key_index != std::string::npos)
		{
			channel_name = channel_name.substr(0, assimp_key_index - 1);
		}
		aiNode_by_channel[channel_name].push_back(channel);
	}

	std::map<double, std::vector<Animation::Channel>> keyframes;
	//Merge channels with same name
	for (auto& channel_set : aiNode_by_channel)
	{
		std::map<double, float3> channel_translations;		
		std::map<double, Quat> channel_rotations;
		for (auto channel : channel_set.second)
		{
			GetChannelTranslations(channel, channel_translations);
			GetChannelRotations(channel, channel_rotations);
		}

		assert(animation->mDuration == (int)animation->mDuration);
		
		if (channel_translations.size() <= 1)
		{
			channel_translations[0] = float3::zero;
			channel_translations[animation->mDuration] = float3::zero;
		}

		if (channel_rotations.size() <= 1)
		{
			channel_rotations[0] = Quat::identity;
			channel_rotations[animation->mDuration] = Quat::identity;
		}

		assert(channel_translations.find(0) != channel_translations.end());
		assert(channel_translations.find(animation->mDuration) != channel_translations.end());
		size_t current_keyframe = 1;
		size_t last_keyframe_sample = 0;
		while (current_keyframe <= animation->mDuration)
		{
			if (channel_translations.find(current_keyframe) != channel_translations.end())
			{
				last_keyframe_sample = current_keyframe;
			}
			else
			{
				size_t next_keyframe_sample = current_keyframe;
				while (next_keyframe_sample <= animation->mDuration && channel_translations.find(next_keyframe_sample) == channel_translations.end())
				{
					++next_keyframe_sample;
				}
				
				size_t num_interpolated_samples = next_keyframe_sample - last_keyframe_sample;
				assert(num_interpolated_samples >= 2);
				for (size_t current_interpolated_frame = 1; current_interpolated_frame < num_interpolated_samples; ++current_interpolated_frame)
				{
					float lambda = current_interpolated_frame/ num_interpolated_samples;
					channel_translations[current_interpolated_frame + last_keyframe_sample] = Utils::Interpolate(
						channel_translations[last_keyframe_sample],
						channel_translations[next_keyframe_sample],
						lambda
					);
				}
				current_keyframe = next_keyframe_sample;
			}
			++current_keyframe;
		}


		assert(channel_rotations.find(0) != channel_rotations.end());
		if (channel_rotations.find(animation->mDuration) == channel_rotations.end())
		{
			int x = 0;
		}
		assert(channel_rotations.find(animation->mDuration) != channel_rotations.end());
		current_keyframe = 1;
		last_keyframe_sample = 0;
		while (current_keyframe <= animation->mDuration)
		{
			if (channel_rotations.find(current_keyframe) != channel_rotations.end())
			{
				last_keyframe_sample = current_keyframe;
			}
			else
			{
				size_t next_keyframe_sample = current_keyframe;
				while (next_keyframe_sample <= animation->mDuration && channel_rotations.find(next_keyframe_sample) == channel_rotations.end())
				{
					++next_keyframe_sample;
				}

				size_t num_interpolated_samples = next_keyframe_sample - last_keyframe_sample;
				assert(num_interpolated_samples >= 2);
				for (size_t current_interpolated_frame = 1; current_interpolated_frame < num_interpolated_samples; ++current_interpolated_frame)
				{
					float lambda = current_interpolated_frame / num_interpolated_samples;
					channel_rotations[current_interpolated_frame + last_keyframe_sample] = Utils::Interpolate(
						channel_rotations[last_keyframe_sample],
						channel_rotations[next_keyframe_sample],
						lambda
					);
				}
				current_keyframe = next_keyframe_sample;
			}
			++current_keyframe;
		}


		for (size_t i = 0; i < animation->mDuration; ++i)
		{
			Animation::Channel imported_channel{ channel_set.first, scale_factor*channel_translations[i], channel_rotations[i] };
			keyframes[i].push_back(imported_channel);
		}
	}

	own_format_animation.keyframes.reserve(keyframes.size());
	for (auto & keyframe : keyframes)
	{
		own_format_animation.keyframes.push_back({static_cast<float>(keyframe.first), keyframe.second});
	}

}

void AnimationImporter::GetChannelTranslations (const aiNodeAnim* sample, std::map<double, float3>& sample_translations) const
{
	for (size_t j = 0; j < sample->mNumPositionKeys; j++)
	{
		if (sample->mPositionKeys[j].mTime >= 0)
		{
			aiVector3D position = sample->mPositionKeys[j].mValue;
			sample_translations[sample->mPositionKeys[j].mTime] = float3(position.x, position.y, position.z);
		}
	}
}

void AnimationImporter::GetChannelRotations(const aiNodeAnim* sample, std::map<double, Quat>& sample_rotations) const
{
	for (size_t j = 0; j < sample->mNumRotationKeys; j++)
	{
		if (sample->mRotationKeys[j].mTime >= 0)
		{
			aiQuaternion rotation = sample->mRotationKeys[j].mValue;
			Quat rotation_quat = Quat(rotation.x, rotation.y, rotation.z, rotation.w);

			sample_rotations[sample->mRotationKeys[j].mTime] = rotation_quat;
		}
	}
}

void AnimationImporter::SaveBinary(const Animation & animation, const std::string & output_file) const
{


	uint32_t num_channels = animation.keyframes.size();

	// number of keyframes +  name size + name + duration
	uint32_t size = sizeof(uint32_t)*2 + animation.name.size() + sizeof(float);


	for (auto & keyframe : animation.keyframes)
	{
		//Number of channels + frame 
		size += sizeof(uint32_t) + sizeof(float);

		for (auto & channel : keyframe.channels)
		{
			//name size + name + position
			size += sizeof(uint32_t)  + channel.name.size() + sizeof(float3) + sizeof(Quat);
		}
	}

	char* data = new char[size]; // Allocate
	char* cursor = data;

	uint32_t name_size = animation.name.size();
	memcpy(cursor, &name_size, sizeof(uint32_t));
	cursor += sizeof(uint32_t);


	memcpy(cursor, animation.name.data(), name_size);
	cursor += name_size;


	memcpy(cursor, &animation.duration, sizeof(float));
	cursor += sizeof(float); // Store duration

	memcpy(cursor, &num_channels, sizeof(uint32_t));
	cursor += sizeof(uint32_t); // Store channels



	for (auto & keyframe : animation.keyframes)
	{
		memcpy(cursor, &keyframe.frame, sizeof(float));
		cursor += sizeof(float);

		uint32_t number_channels = keyframe.channels.size();
		memcpy(cursor, &number_channels, sizeof(uint32_t));
		cursor += sizeof(uint32_t);

		for (auto & channel : keyframe.channels)
		{
			uint32_t name_size = channel.name.size();
			memcpy(cursor, &name_size, sizeof(uint32_t));
			cursor += sizeof(uint32_t);

			memcpy(cursor, channel.name.data(), name_size);
			cursor += name_size;

			memcpy(cursor, &channel.translation, sizeof(float3));
			cursor += sizeof(float3);

			memcpy(cursor, &channel.rotation, sizeof(Quat));
			cursor += sizeof(Quat);
		}
	}

	App->filesystem->Save(output_file.c_str(), data, size);
	free(data);
}
