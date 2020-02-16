#ifndef _ANIMATIONIMPORTER_H_
#define _ANIMATIONIMPORTER_H_

#include <string>
#include <unordered_map>
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
};
#endif

