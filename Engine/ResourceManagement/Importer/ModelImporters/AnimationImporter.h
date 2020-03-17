#ifndef _ANIMATIONIMPORTER_H_
#define _ANIMATIONIMPORTER_H_

#include <string>
#include <unordered_map>
#include <map>
#include <ResourceManagement/Resources/Animation.h>

struct aiAnimation;
struct aiScene;
struct aiNodeAnim;
class Animation;

class AnimationImporter
{
public:

	AnimationImporter() = default;
	~AnimationImporter() = default;

	bool ImportAnimation(const aiScene* scene, const aiAnimation* animation, std::string& output_file) const;

private:
	void GetCleanAnimation(const aiAnimation* animation, Animation & own_format_animation, float scale_factor) const;
	void GetChannelTranslations(const aiNodeAnim* sample, float animation_duration, std::map<double, float3>& sample_translations) const;
	void GetChannelRotations(const aiNodeAnim* sample, float animation_duration, std::map<double, Quat>& sample_rotations) const;
	void SaveBinary(const Animation & own_format_animation, const std::string & output_file) const;

	const std::string LIBRARY_ANIMATION_FOLDER{ "Library/Animations" };
};
#endif

