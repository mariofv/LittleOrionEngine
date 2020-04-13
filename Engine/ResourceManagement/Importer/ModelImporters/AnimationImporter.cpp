#include "AnimationImporter.h"

#include "Helper/Utils.h"
#include "Main/Application.h"
#include "Module/ModuleFileSystem.h"
#include "ResourceManagement/Importer/ModelImporters/SkeletonImporter.h"
#include "ResourceManagement/Resources/Animation.h"

#include <assimp/scene.h>
#include <cmath>
#include <map>

bool AnimationImporter::ImportAnimation(const aiScene* scene, const aiAnimation* animation, const std::string& imported_file, std::string& exported_file , float unit_scale_factor) const
{
	Animation own_format_animation(0,"");
	GetCleanAnimation(scene->mRootNode, animation, own_format_animation, unit_scale_factor);
	own_format_animation.frames = static_cast<float>(animation->mDuration);
	own_format_animation.frames_per_second = static_cast<float>(animation->mTicksPerSecond);
	own_format_animation.name = std::string(animation->mName.C_Str());

	std::map<const std::string, std::vector<const aiNode *>> nodes;
	GetAssimpNodeTansformationOutSideChannels(scene->mRootNode, own_format_animation, nodes);

	ApplyNodeTansformationOutSideChannels(nodes, unit_scale_factor, own_format_animation);
	exported_file = SaveMetaFile(imported_file, ResourceType::ANIMATION);
	SaveBinary(own_format_animation, exported_file, imported_file);

	return true;
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
			assimp_transform = assimp_transform * SkeletonImporter::GetTransform(second->mTransformation);
		}
		assimp_transform.Decompose(channel.translation, channel.rotation, scale);
		channel.translation *= unit_scale_factor;
		for (auto & keyframe : own_format_animation.keyframes)
		{
			keyframe.channels.push_back(channel);
		}
	}
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
		float4x4 accumulated_assimp_transformation;
		GetAcumulatedAssimpTransformations(channel_set, root_node, accumulated_assimp_transformation);
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

			float4x4 animation_transform = float4x4::FromTRS(translation, rotation, float3::one);
			animation_transform = accumulated_assimp_transformation * animation_transform;
				
			float3 euler_rotation = animation_transform.ToEulerXYX();
			rotation = Quat::FromEulerXYX(euler_rotation.x, euler_rotation.y, euler_rotation.z);
			if (!is_translated)
			{
				translation = animation_transform.Col3(3);
			}
			translation *= scale_factor;

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

	App->filesystem->Save(exported_file.c_str(), data, size);
	App->filesystem->Save(imported_file.c_str(), data, size);
	free(data);
}

void AnimationImporter::GetAcumulatedAssimpTransformations(const std::pair<std::string, std::vector<aiNodeAnim *>> & channel_pair, const aiNode* root_node, float4x4 & accumulated_transformation) const
{
	const aiNode* current_node = root_node->FindNode(aiString(channel_pair.first));
	std::vector<const aiNode*> assimp_hierarchy_nodes;

	const aiNode* next_node = current_node->mParent;
	bool is_assimp_node = std::string(next_node->mName.C_Str()).find("$Assimp") != std::string::npos;
	while (is_assimp_node)
	{
		assimp_hierarchy_nodes.push_back(next_node);
		next_node = next_node->mParent;
		is_assimp_node = std::string(next_node->mName.C_Str()).find("$Assimp") != std::string::npos;

	}
	aiMatrix4x4 accumulated_assimp_local_transformation;
	for (int i = assimp_hierarchy_nodes.size() - 1; i >= 0; --i)
	{
		const aiNode* hierarchy_node = assimp_hierarchy_nodes[i];
		auto it = std::find_if(channel_pair.second.begin(), channel_pair.second.end(), [hierarchy_node](const aiNodeAnim * node)
		{
			return node->mNodeName == hierarchy_node->mName;
		}
		);
		if (it == channel_pair.second.end())
		{
			accumulated_assimp_local_transformation = accumulated_assimp_local_transformation * assimp_hierarchy_nodes[i]->mTransformation;
		}
	}

	accumulated_transformation = SkeletonImporter::GetTransform(accumulated_assimp_local_transformation);
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
