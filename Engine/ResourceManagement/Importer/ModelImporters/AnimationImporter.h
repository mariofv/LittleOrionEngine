#ifndef _ANIMATIONIMPORTER_H_
#define _ANIMATIONIMPORTER_H_

#include <string>
struct aiAnimation;
struct aiScene;
class AnimationImporter
{
public:
	AnimationImporter() = default;
	~AnimationImporter() = default;

	bool ImportAnimation(const aiScene* scene, const aiAnimation* mesh, std::string& output_file) const;
};
#endif

