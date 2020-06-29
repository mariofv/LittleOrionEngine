#include "AnimationImporter.h"

#include "Helper/Utils.h"
#include "Main/Application.h"
#include "Module/ModuleFileSystem.h"
#include "ResourceManagement/Importer/ModelImporters/SkeletonImporter.h"
#include "ResourceManagement/Resources/Animation.h"

#include <assimp/scene.h>
#include <cmath>
#include <map>

FileData AnimationImporter::ExtractData(Path& assets_file_path, const Metafile& metafile) const
{
	return assets_file_path.GetFile()->Load();
}

FileData AnimationImporter::ExtractAnimationFromAssimp(const aiScene* scene, const aiAnimation* animation, float unit_scale_factor) const
{
	Animation own_format_animation;

	GetCleanAnimation(scene->mRootNode, animation, own_format_animation, unit_scale_factor);

	own_format_animation.frames = static_cast<float>(animation->mDuration);
	own_format_animation.frames_per_second = static_cast<float>(animation->mTicksPerSecond);
	own_format_animation.name = std::string(animation->mName.C_Str());

	std::map<const std::string, std::vector<const aiNode *>> nodes;

	return CreateBinary(own_format_animation);
}


void AnimationImporter::GetCleanAnimation(const aiNode* root_node, const aiAnimation* animation, Animation& own_format_animation, float unit_scale_factor) const
{
	float animation_duration = static_cast<float>(animation->mDuration);
	std::map<std::string, std::vector<aiNodeAnim*>> aiNode_by_channel;

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
		std::map<size_t, float4x4> channel_transform;
		for (auto channel : channel_set.second)
		{
			GetChannelTransform(channel, static_cast<size_t>(animation_duration), channel_transform);
		}

		for (size_t i = 0; i < animation_duration; ++i)
		{
			float3 euler_rotation = channel_transform[i].ToEulerXYZ();
			Quat rotation = Quat::FromEulerXYZ(euler_rotation.x, euler_rotation.y, euler_rotation.z);
			float3 translation = channel_transform[i].Col3(3) * unit_scale_factor;

			Animation::Channel imported_channel{ channel_set.first, translation, rotation };
			keyframes[i].push_back(imported_channel);
		}
	}

	own_format_animation.keyframes.reserve(keyframes.size());
	for (auto & keyframe : keyframes)
	{
		own_format_animation.keyframes.push_back({ static_cast<float>(keyframe.first), keyframe.second });
	}
}

void AnimationImporter::GetChannelTransform( const aiNodeAnim * sample, size_t animation_duration, std::map<size_t, float4x4>& sample_transform ) const
{
	float3 position = float3::zero;
	Quat rotation = Quat::identity;
	for (size_t j = 0; j <= animation_duration; j++)
	{
		if (sample->mNumPositionKeys >= j && sample->mPositionKeys[j].mTime >= 0.0f)
		{
			aiVector3D position_assimp = sample->mPositionKeys[j].mValue;
			position = float3(position_assimp.x, position_assimp.y, position_assimp.z);
		}
		if (sample->mNumRotationKeys >= j &&sample->mRotationKeys[j].mTime >= 0.0f )
		{
			aiQuaternion rotation_assimp = sample->mRotationKeys[j].mValue;
			rotation = Quat(rotation_assimp.x, rotation_assimp.y, rotation_assimp.z, rotation_assimp.w);
		}

		sample_transform[j] =  sample_transform[j] *  float4x4::FromTRS(position,rotation, float3::one);
	}
}

FileData AnimationImporter::CreateBinary(const Animation& animation) const
{
	// number of keyframes +  name size + name + duration
	uint32_t size = sizeof(uint32_t) * 2 + animation.name.size() + sizeof(float);

	for (auto & keyframe : animation.keyframes)
	{
		//Number of channels + frame + ticks_per_second 
		size += sizeof(uint32_t) + sizeof(float) + sizeof(float);

		for (auto & channel : keyframe.channels)
		{
			//name size + name + translation + rotation
			size += sizeof(uint32_t) + channel.name.size() + sizeof(float3) + sizeof(Quat);
		}
	}

	char* data = new char[size]; // Allocate
	char* cursor = data;

	uint32_t name_size = animation.name.size();
	memcpy(cursor, &name_size, sizeof(uint32_t));
	cursor += sizeof(uint32_t);


	memcpy(cursor, animation.name.data(), name_size);
	cursor += name_size;


	memcpy(cursor, &animation.frames, sizeof(float));
	cursor += sizeof(float); // Store duration

	memcpy(cursor, &animation.frames_per_second, sizeof(float));
	cursor += sizeof(float); // Store duration

	uint32_t num_keyframes = animation.keyframes.size();
	memcpy(cursor, &num_keyframes, sizeof(uint32_t));
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

	FileData animation_data{data, size};
	return animation_data;
}
