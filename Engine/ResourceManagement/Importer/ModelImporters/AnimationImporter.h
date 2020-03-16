#ifndef _ANIMATIONIMPORTER_H_
#define _ANIMATIONIMPORTER_H_

#include <string>
#include <unordered_map>
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
	void GetCleanAnimation(const aiAnimation* animation, Animation & own_format_animation) const ;
	void TransformPositions(const aiNodeAnim* sample, std::unordered_map<double, float3>& sample_translations, std::unordered_map<double, Quat>& sample_rotations) const;
	void SaveBinary(const Animation & own_format_animation, const std::string & output_file) const;

	const std::string LIBRARY_ANIMATION_FOLDER{ "Library/Animations" };
};
#endif

