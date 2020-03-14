#ifndef _ANIMATIONIMPORTER_H_
#define _ANIMATIONIMPORTER_H_

#include "ResourceManagement/Resources/Animation.h"
#include "ResourceManagement/Importer/Importer.h"

#include <string>
#include <unordered_map>

struct aiAnimation;
struct aiScene;
struct aiNodeAnim;
class Animation;

class AnimationImporter : public Importer
{
public:

	AnimationImporter() = default;
	~AnimationImporter() = default;

	bool ImportAnimation(const aiScene* scene, const aiAnimation* animation, const std::string& imported_file, std::string& exported_file) const;

private:
	void GetCleanAnimation(const aiAnimation* animation, Animation& own_format_animation) const ;
	void TransformPositions(const aiNodeAnim* ai_node, std::unordered_map<double, float4x4>& frames) const;
	void SaveBinary(const Animation& own_format_animation, const std::string& exported_file, const std::string& imported_file) const;


	const std::string LIBRARY_ANIMATION_FOLDER{ "Library/Animations" };
};
#endif

