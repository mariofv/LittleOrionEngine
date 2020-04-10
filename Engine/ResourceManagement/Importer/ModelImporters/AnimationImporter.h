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

	AnimationImporter() = default;
	~AnimationImporter() = default;

	bool ImportAnimation(const aiScene* scene, const aiAnimation* animation, const std::string& imported_file, std::string& exported_file, float unit_scale_factor) const;


private:
	void GetCleanAnimation(const aiNode* root_node, const aiAnimation* animation, Animation & own_format_animation, float scale_factor) const;
	void GetChannelTranslations(const aiNodeAnim* sample, std::map<size_t, float3>& sample_translations) const;
	void GetChannelRotations(const aiNodeAnim* sample, std::map<size_t, Quat>& sample_rotations) const;
	void SaveBinary(const Animation& animation, const std::string& exported_file, const std::string& imported_file) const;

	void GetAcumulatedAssimpTransformations(const std::pair<std::string, std::vector<aiNodeAnim *>> & channel_pair, const aiNode* root_node, float4x4 & accumulated_transformation) const;
	void GetAssimpNodeTansformationOutSideChannels(const aiNode* root_node, const Animation& animation, std::map<const std::string, std::vector<const aiNode *>>  & nodes) const;
	void ApplyNodeTansformationOutSideChannels(std::map<const std::string, std::vector<const aiNode *>> &nodes, float unit_scale_factor, Animation &own_format_animation) const;
}
#endif

