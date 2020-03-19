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

	bool ImportAnimation(const aiScene* scene, const aiAnimation* animation, const std::string& imported_file, std::string& exported_file) const;

private:
	void GetCleanAnimation(const aiNode* root_node, const aiAnimation* animation, Animation & own_format_animation, float scale_factor) const;
	void GetChannelTranslations(const aiNodeAnim* sample, float animation_duration, std::map<double, float3>& sample_translations) const;
	void GetChannelRotations(const aiNodeAnim* sample, float animation_duration, std::map<double, Quat>& sample_rotations) const;
	void SaveBinary(const Animation & own_format_animation, const std::string & output_file) const;

};
#endif

