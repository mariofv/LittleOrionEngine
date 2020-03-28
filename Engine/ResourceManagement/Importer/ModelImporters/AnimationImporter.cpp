#include "AnimationImporter.h"

#include "Helper/Utils.h"
#include "Main/Application.h"
#include "Module/ModuleFileSystem.h"
#include "ResourceManagement/Importer/ModelImporters/SkeletonImporter.h"
#include "ResourceManagement/Resources/Animation.h"

#include <assimp/scene.h>
#include <cmath>
#include <map>

bool AnimationImporter::ImportAnimation(const aiScene* scene, const aiAnimation* animation, const std::string& imported_file, std::string& exported_file) const
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


	Animation own_format_animation(0,"");
	GetCleanAnimation(scene->mRootNode, animation, own_format_animation, unit_scale_factor);
	own_format_animation.frames = static_cast<float>(animation->mDuration);
	own_format_animation.frames_per_second = static_cast<float>(animation->mTicksPerSecond);
	own_format_animation.name = std::string(animation->mName.C_Str());

	exported_file = SaveMetaFile(imported_file, ResourceType::ANIMATION);
	SaveBinary(own_format_animation, exported_file, imported_file);

	return true;
}

void AnimationImporter::GetCleanAnimation(const aiNode* root_node, const aiAnimation* animation, Animation& own_format_animation, float scale_factor) const
{
	assert(animation->mDuration == (int)animation->mDuration);

	std::map<std::string, std::vector<aiNodeAnim *>> aiNode_by_channel;

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
		const aiNode* current_node = root_node->FindNode(aiString(channel_set.first));
		std::vector<const aiNode*> assimp_hierarchy_nodes;

		const aiNode* next_node = current_node->mParent;
		std::string next_node_name(next_node->mName.C_Str());
		while (next_node_name.find("$Assimp") != std::string::npos)
		{
			assimp_hierarchy_nodes.push_back(next_node);
			next_node = next_node->mParent;
			next_node_name = next_node->mName.C_Str();
		}
		aiMatrix4x4 accumulated_assimp_local_transformation;
		for (int i = assimp_hierarchy_nodes.size() - 1; i >= 0; --i)
		{
			const aiNode* hierarchy_node = assimp_hierarchy_nodes[i];
			auto it = std::find_if(channel_set.second.begin(), channel_set.second.end(), [hierarchy_node](const aiNodeAnim * node)
			{
				return node->mNodeName == hierarchy_node->mName;
			}
			);
			if (it == channel_set.second.end())
			{
				accumulated_assimp_local_transformation = accumulated_assimp_local_transformation * assimp_hierarchy_nodes[i]->mTransformation;
			}
		}

		float4x4 accumulated_assimp_transformation = SkeletonImporter::GetTransform(accumulated_assimp_local_transformation);

		std::map<size_t, float3> channel_translations;
		std::map<size_t, Quat> channel_rotations;
		for (auto channel : channel_set.second)
		{
			GetChannelTranslations(channel, channel_translations);
			GetChannelRotations(channel, channel_rotations);

		}

		float animation_duration = animation->mDuration;
		for (size_t i = 0; i <= animation_duration; ++i)
		{
			bool is_translated;
			float3 translation;
			if (channel_translations.size() > 1 && channel_translations.find(i) != channel_translations.end())
			{
				is_translated = true;
				translation = channel_translations[i];
			}
			else
			{
				is_translated = false;
				translation = channel_translations[0];
			}

			bool is_rotated;
			Quat rotation;
			if (channel_rotations.size() > 1 && channel_rotations.find(i) != channel_rotations.end())
			{
				is_rotated = true;
				rotation = channel_rotations[i];
			}
			else
			{
				is_rotated = false;
				rotation = channel_rotations[0];
			}
			if (is_rotated || is_translated)
			{
				float4x4 animation_transform = float4x4::FromTRS(translation, rotation, float3::one);
				animation_transform = accumulated_assimp_transformation * animation_transform;
				float3 euler_rotation = animation_transform.ToEulerXYX();
				rotation = Quat::FromEulerXYX(euler_rotation.x, euler_rotation.y, euler_rotation.z);
				translation *= scale_factor;
				Animation::Channel imported_channel{ channel_set.first, is_translated, translation, is_rotated, rotation };
				keyframes[i].push_back(imported_channel);
			}
		}
	}

	own_format_animation.keyframes.reserve(keyframes.size());
	for (auto & keyframe : keyframes)
	{
		own_format_animation.keyframes.push_back({ static_cast<float>(keyframe.first), keyframe.second });
	}

}

void AnimationImporter::GetChannelTranslations(const aiNodeAnim* sample,std::map<size_t, float3>& sample_translations) const
{
	for (size_t j = 0; j < sample->mNumPositionKeys; j++)
	{
		if (sample->mPositionKeys[j].mTime >= 0)
		{
			// Some animation sample times are stored with an small rounding error, so we need to round them
			double integer_time = std::round(sample->mPositionKeys[j].mTime);
			aiVector3D position = sample->mPositionKeys[j].mValue;
			sample_translations[integer_time] = float3(position.x, position.y, position.z);
		}
	}

}

void AnimationImporter::GetChannelRotations(const aiNodeAnim* sample, std::map<size_t, Quat>& sample_rotations) const
{
	for (size_t j = 0; j < sample->mNumRotationKeys; j++)
	{
		if (sample->mRotationKeys[j].mTime >= 0)
		{
			// Some animation sample times are stored with an small rounding error, so we need to round them
			size_t integer_time = std::round(sample->mRotationKeys[j].mTime);
			aiQuaternion rotation = sample->mRotationKeys[j].mValue;
			sample_rotations[integer_time] = Quat(rotation.x, rotation.y, rotation.z, rotation.w);
		}
	}
}

void AnimationImporter::SaveBinary(const Animation& animation, const std::string& exported_file, const std::string& imported_file) const
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
			size += sizeof(uint32_t) + channel.name.size() + sizeof(bool) + sizeof(float3) + sizeof(bool) + sizeof(Quat);
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

			memcpy(cursor, &channel.is_translated, sizeof(bool));
			cursor += sizeof(bool);

			memcpy(cursor, &channel.translation, sizeof(float3));
			cursor += sizeof(float3);

			memcpy(cursor, &channel.is_rotated, sizeof(bool));
			cursor += sizeof(bool);

			memcpy(cursor, &channel.rotation, sizeof(Quat));
			cursor += sizeof(Quat);
		}
	}

	App->filesystem->Save(exported_file.c_str(), data, size);
	App->filesystem->Save(imported_file.c_str(), data, size);
	free(data);
}