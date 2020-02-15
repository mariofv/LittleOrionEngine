#ifndef _ANIMATIONIMPORTER_H_
#define _ANIMATIONIMPORTER_H_

#include <string>
#include <unordered_map>
struct aiAnimation;
struct aiScene;
struct aiNodeAnim;
class AnimationImporter
{
public:

	AnimationImporter() = default;
	~AnimationImporter() = default;

	bool ImportAnimation(const aiScene* scene, const aiAnimation* animation, std::string& output_file) const;

private:
	void GetCleanAnimation(const aiAnimation* animation, std::unordered_map<std::string, std::vector<aiNodeAnim *>> & aiNode_by_channel) const ;
};
#endif

