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
	output_file = LIBRARY_ANIMATION_FOLDER + "/" + std::to_string(random())+ "_"+ own_format_animation.name + ".anim";

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
	for (auto & nodes : aiNode_by_channel)
	{
		std::unordered_map<double, float4x4>  frames;
		
		for (auto ai_node : nodes.second)
		{
			TransformPositions(ai_node, frames);
		}

		for (auto & frame : frames)
		{

			Animation::Channel channel{ nodes.first, frame.second };
			keyframes[frame.first].push_back(channel);
		}
	}

	own_format_animation.keyframes.reserve(keyframes.size());
	for (auto & keyframe : keyframes)
	{
		own_format_animation.keyframes.push_back({static_cast<float>(keyframe.first), keyframe.second});
	}

}

void AnimationImporter::TransformPositions(const aiNodeAnim * ai_node, std::unordered_map<double, float4x4> & frames) const
{

	for (size_t j = 0; j < ai_node->mNumScalingKeys; j++)
	{
		if (ai_node->mScalingKeys[j].mTime >= 0)
		{
			aiVector3D scale = ai_node->mScalingKeys[j].mValue;
			if (frames.find(ai_node->mScalingKeys[j].mTime) == frames.end())
			{
				frames[ai_node->mScalingKeys[j].mTime] = float4x4::identity;
			}
			frames[ai_node->mScalingKeys[j].mTime].Scale(scale.x, scale.y, scale.z);
		}
	}

	for (size_t j = 0; j < ai_node->mNumRotationKeys; j++)
	{

		if (ai_node->mRotationKeys[j].mTime >= 0)
		{

			aiQuaternion rotation = ai_node->mRotationKeys[j].mValue;
			float4x4 rotation_matrix = float4x4::FromQuat(Quat(rotation.x, rotation.y, rotation.z, rotation.w));

			if (frames.find(ai_node->mRotationKeys[j].mTime) == frames.end())
			{
				frames[ai_node->mRotationKeys[j].mTime] = float4x4::identity;
			}
			frames[ai_node->mRotationKeys[j].mTime] = frames[ai_node->mRotationKeys[j].mTime] * rotation_matrix;
		}
	}

	for (size_t j = 0; j < ai_node->mNumPositionKeys; j++)
	{

		if (ai_node->mPositionKeys[j].mTime >= 0)
		{
			aiVector3D position = ai_node->mPositionKeys[j].mValue;
			if (frames.find(ai_node->mPositionKeys[j].mTime) == frames.end())
			{
				frames[ai_node->mPositionKeys[j].mTime] = float4x4::identity;
				frames[ai_node->mPositionKeys[j].mTime].SetTranslatePart(position.x, position.y, position.z);
			}
			else
			{
				frames[ai_node->mPositionKeys[j].mTime].Translate(position.x, position.y, position.z);
			}
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
			size += sizeof(uint32_t)  + channel.name.size() + sizeof(float4x4);
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

			memcpy(cursor, &channel.position, sizeof(float4x4));
			cursor += sizeof(float4x4);
		}
	}

	App->filesystem->Save(output_file.c_str(), data, size);
	free(data);
}
