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
	GetAssimpNodeTansformationOutSideChannels(scene->mRootNode, own_format_animation, nodes);

	ApplyNodeTansformationOutSideChannels(nodes, unit_scale_factor, own_format_animation);

	return CreateBinary(own_format_animation);
}


void AnimationImporter::GetCleanAnimation(const aiNode* root_node, const aiAnimation* animation, Animation& own_format_animation, float unit_scale_factor) const
{
	float animation_duration = animation->mDuration;
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
			float4x4 pre_transform;
			GetAcumulatedAssimpTransformations(channel, channel_set.second, root_node, pre_transform);
			GetChannelTransform(pre_transform, channel, animation_duration, channel_transform);
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

void AnimationImporter::ApplyNodeTansformationOutSideChannels(std::map<const std::string, std::vector<const aiNode *>> &nodes, float unit_scale_factor, Animation &own_format_animation) const
{
	for (auto& pair : nodes)
	{
		Animation::Channel channel;
		channel.name = pair.first;
		float4x4 assimp_transform;

		float3 scale;
		for (auto & second : pair.second)
		{
			assimp_transform = assimp_transform * Utils::GetTransform(second->mTransformation);
		}
		assimp_transform.Decompose(channel.translation, channel.rotation, scale);
		channel.translation *= unit_scale_factor;
		for (auto & keyframe : own_format_animation.keyframes)
		{
			keyframe.channels.push_back(channel);
		}
	}
}

void AnimationImporter::GetChannelTransform(const float4x4 & pre_transform, const aiNodeAnim * sample, size_t animation_duration, std::map<size_t, float4x4>& sample_transform ) const
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

		sample_transform[j] =  sample_transform[j] * pre_transform * float4x4::FromTRS(position,rotation, float3::one);
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

void AnimationImporter::GetAcumulatedAssimpTransformations(const aiNodeAnim* animation_channel, const std::vector<aiNodeAnim*> & channel_vector, const aiNode* root_node, float4x4& pre_transform) const
{
	const aiNode* node_parent = root_node->FindNode(animation_channel->mNodeName)->mParent;
	auto it = std::find_if(channel_vector.begin(), channel_vector.end(), [node_parent](const aiNodeAnim * node)
	{
		return node->mNodeName == node_parent->mName;
	});

	bool assimp_node = std::string(node_parent->mName.C_Str()).find("$Assimp") != std::string::npos;
	bool not_added = it == channel_vector.end();
	pre_transform = float4x4::identity;

	while (assimp_node && not_added && node_parent!= root_node)
	{
		pre_transform =  Utils::GetTransform(node_parent->mTransformation) * pre_transform;
		node_parent = node_parent->mParent;
		assimp_node = std::string(node_parent->mName.C_Str()).find("$Assimp") != std::string::npos;
		auto it = std::find_if(channel_vector.begin(), channel_vector.end(), [node_parent](const aiNodeAnim * node)
		{
			return node->mNodeName == node_parent->mName;
		});
		not_added = it == channel_vector.end();
	}
}

void AnimationImporter::GetAssimpNodeTansformationOutSideChannels(const aiNode * root_node, const Animation& animation, std::map<const std::string, std::vector<const aiNode *>> & nodes) const
{

	for (size_t i = 0; i < root_node->mNumChildren; i++)
	{
		const aiNode * child = root_node->mChildren[i];
		std::string node_name(child->mName.C_Str());

		bool assimp_node = node_name.find("$Assimp") != std::string::npos;
		bool part_of_channels = false;
		size_t j = 0;
		auto & channels_vector = animation.keyframes[0].channels;
		while (j < channels_vector.size() && !part_of_channels && assimp_node)
		{
			if (std::string(child->mName.C_Str()).find(channels_vector[j].name) != std::string::npos)
			{
				part_of_channels = true;
			}
			++j;
		}

		if (assimp_node && !part_of_channels)
		{
			std::string channel_name(child->mName.C_Str());

			size_t assimp_key_index = channel_name.find("$Assimp");
			if (assimp_key_index != std::string::npos)
			{
				channel_name = channel_name.substr(0, assimp_key_index - 1);
			}
			nodes[channel_name].push_back(child);
		}
		GetAssimpNodeTansformationOutSideChannels(child, animation, nodes);
	}
}
