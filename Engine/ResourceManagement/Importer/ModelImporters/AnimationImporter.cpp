#include "AnimationImporter.h"

#include <assimp/scene.h>
#include <ResourceManagement/Resources/Animation.h>

bool AnimationImporter::ImportAnimation(const aiScene* scene, const aiAnimation* animation, std::string& output_file) const
{
	Animation own_format_animation("", "");
	GetCleanAnimation(animation, own_format_animation);


	own_format_animation.duration = animation->mDuration;
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


	//Merge channels
	for (auto & nodes : aiNode_by_channel)
	{
		Animation::Channel channel{ nodes.first };

		std::unordered_map<float, float4x4>  frame;
		for (auto ai_node : nodes.second)
		{

			for (size_t j = 0; j < ai_node->mNumPositionKeys; j++)
			{
				if (ai_node->mScalingKeys[j].mTime < 0) {
					return;
				}
				aiVector3D position = ai_node->mPositionKeys[j].mValue;
				frame[ai_node->mPositionKeys[j].mTime].Translate(position.x, position.y, position.z);

			}
			for (size_t j = 0; j < ai_node->mNumRotationKeys; j++)
			{
				if (ai_node->mScalingKeys[j].mTime < 0) {
					return;
				}
				aiQuaternion rotation = ai_node->mRotationKeys[j].mValue;
				float4x4 rotation_matrix = float4x4::FromQuat(Quat(rotation.x, rotation.y, rotation.z, rotation.w));
				frame[ai_node->mRotationKeys[j].mTime] = frame[ai_node->mRotationKeys[j].mTime] * rotation_matrix;
			}
			for (size_t j = 0; j < ai_node->mNumScalingKeys; j++)
			{
				if (ai_node->mScalingKeys[j].mTime < 0) {
					return;
				}
				aiVector3D scale = ai_node->mScalingKeys[j].mValue;
				frame[ai_node->mScalingKeys[j].mTime].Scale(scale.x, scale.y, scale.z);
			}

		}

		for (auto & positions : frame)
		{

			channel.positions.push_back({ positions.first, positions.second });
		}

		own_format_animation.channels.push_back(channel);
	}
}
