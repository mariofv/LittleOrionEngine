#ifndef _ANIMATIONIMPORTER_H_
#define _ANIMATIONIMPORTER_H_

#include "ResourceManagement/Resources/Animation.h"
#include "ResourceManagement/Importer/Importer.h"

#include <string>
#include <map>

struct aiAnimation;
struct aiScene;
struct aiNodeAnim;
struct aiNode;
class Animation;

class AnimationImporter : public Importer
{
public:

	AnimationImporter() : Importer(ResourceType::ANIMATION) {};
	~AnimationImporter() = default;

	FileData ExtractAnimationFromAssimp(const aiScene* scene, const aiAnimation* animation, float unit_scale_factor) const;

private:
	FileData CreateBinary(const Animation& own_format_animation) const;

	void GetCleanAnimation(const aiNode* root_node, const aiAnimation* animation, Animation& own_format_animation, float scale_factor) const;
	void GetChannelTransform(const float4x4 &pre_transform, const aiNodeAnim* sample, size_t animation_duration,std::map<size_t, float4x4>& sample_transform) const;

	void GetAcumulatedAssimpTransformations(const aiNodeAnim * animation_channel, const std::vector<aiNodeAnim *> & channel_vector, const aiNode* root_node, float4x4 & pre_transform) const;
	void GetAssimpNodeTansformationOutSideChannels(const aiNode* root_node, const Animation& animation, std::map<const std::string, std::vector<const aiNode *>>  & nodes) const;
	void ApplyNodeTansformationOutSideChannels(std::map<const std::string, std::vector<const aiNode *>> &nodes, float unit_scale_factor, Animation &own_format_animation) const;
};
#endif

