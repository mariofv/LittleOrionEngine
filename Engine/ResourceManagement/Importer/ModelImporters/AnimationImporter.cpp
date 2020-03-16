#include "AnimationImporter.h"

#include <assimp/scene.h>
#include <ResourceManagement/Resources/Animation.h>

#include <Main/Application.h>
#include <Module/ModuleFileSystem.h>

#include <random>
#include <map>

bool AnimationImporter::ImportAnimation(const aiScene* scene, const aiAnimation* animation, std::string& output_file) const
{
	Animation own_format_animation("", "");
	GetCleanAnimation(animation, own_format_animation);
	own_format_animation.duration = static_cast<float>(animation->mDuration);
	own_format_animation.name = std::string(animation->mName.C_Str());

	std::random_device random;
	App->filesystem->MakeDirectory(LIBRARY_ANIMATION_FOLDER);
	output_file = LIBRARY_ANIMATION_FOLDER + "/" + std::string(scene->mRootNode->mName.C_Str()) + "_" + own_format_animation.name + ".anim";

	SaveBinary(own_format_animation, output_file);
	return true;
}

/*
A channel is all the positions for a joint in each frame.
aiNodeAnim conteins the position in each frame.

Assimp added nodes need to be merge with the real node.

*/

void AnimationImporter::GetCleanAnimation(const aiAnimation* animation, Animation & own_format_animation) const
{
	std::unordered_map<std::string, std::vector<aiNodeAnim *>> aiNode_by_channel;


	//Organize channels
	for (size_t i = 0; i < animation->mNumChannels; i++)
	{
		aiNodeAnim * channel = animation->mChannels[i];
		std::string channel_name(channel->mNodeName.C_Str());

		size_t assimp_key_index = channel_name.find("$Assimp");
		if (assimp_key_index != std::string::npos)
		{
			channel_name = channel_name.substr(0, assimp_key_index - 1);
		}
		aiNode_by_channel[channel_name].push_back(channel);
	}


	std::map<double, std::vector<Animation::Channel>> keyframes;

	//Merge channels
	for (auto& channel : aiNode_by_channel)
	{
		std::unordered_map<double, float3> sample_translations;
		std::unordered_map<double, Quat> sample_rotations;
		
		for (auto sample : channel.second)
		{
			TransformPositions(sample, sample_translations, sample_rotations);
		}

		assert(animation->mDuration == (int)animation->mDuration);
		
		if (sample_translations.size() == 0)
		{
			sample_translations[0] = float3::zero;
		}

		if (sample_rotations.size() == 0) // TODO: Check this
		{
			sample_rotations[0] = Quat::identity;
		}

		float3 last_translation;
		Quat last_rotation;
		for (size_t i = 0; i < animation->mDuration; ++i)
		{
			if (i != 0)
			{
				last_translation = sample_translations[i - 1];
				last_rotation = sample_rotations[i - 1];
			}

			float3 sample_translation = sample_translations.find(i) == sample_translations.end() ? last_translation : sample_translations[i];
			Quat sample_rotation = sample_rotations.find(i) == sample_rotations.end() ? last_rotation : sample_rotations[i];
			Animation::Channel imported_channel{ channel.first, sample_translations[i], sample_rotation};
			keyframes[i].push_back(imported_channel);
		}
	}

	own_format_animation.keyframes.reserve(keyframes.size());
	for (auto & keyframe : keyframes)
	{
		own_format_animation.keyframes.push_back({static_cast<float>(keyframe.first), keyframe.second});
	}

}

void AnimationImporter::TransformPositions
(
	const aiNodeAnim* sample,
	std::unordered_map<double,	float3>& sample_translations,
	std::unordered_map<double, Quat>& sample_rotations
) const
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

	for (size_t j = 0; j < sample->mNumPositionKeys; j++)
	{
		if (sample->mPositionKeys[j].mTime >= 0)
		{
			aiVector3D position = sample->mPositionKeys[j].mValue;
			sample_translations[sample->mPositionKeys[j].mTime] = float3(position.x, position.y, position.z);
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
