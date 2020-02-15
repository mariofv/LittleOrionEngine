#include "AnimationImporter.h"

#include <assimp/scene.h>
#include <ResourceManagement/Resources/Animation.h>

bool AnimationImporter::ImportAnimation(const aiScene* scene, const aiAnimation* animation, std::string& output_file) const
{

	std::unordered_map<std::string, std::vector<aiNodeAnim *>> aiNode_by_channel;
	GetCleanAnimation(animation, aiNode_by_channel);
	Animation own_format_animation("","");

	own_format_animation.duration = animation->mDuration;
	return true;
}

/*
A channel is all the positions for a joint in each frame.
aiNodeAnim conteins the position in each frame.

Assimp added nodes need to be merge with the real node.

*/

void AnimationImporter::GetCleanAnimation(const aiAnimation* animation, std::unordered_map<std::string, std::vector<aiNodeAnim *>> & aiNode_by_channel) const
{
	std::unordered_map<long, float4x4>  time;
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

		for (size_t j = 0; j < channel->mNumRotationKeys; j++)
		{
			aiQuaternion rotation = channel->mRotationKeys[j].mValue;
			time[channel->mRotationKeys[j].mTime].FromQuat(Quat(rotation.x, rotation.y, rotation.z, rotation.w));
		}
		for (size_t j = 0; j < channel->mNumPositionKeys; j++)
		{
			aiVector3D position = channel->mPositionKeys[j].mValue;
			time[channel->mPositionKeys[j].mTime].Translate(position.x, position.y, position.z);
		}

		for (size_t j = 0; j < channel->mNumScalingKeys; j++)
		{
			aiVector3D scale = channel->mScalingKeys[j].mValue;
			time[channel->mScalingKeys[j].mTime].Scale(scale.x, scale.y, scale.z);
		}
	}

	int x = 0;
}
