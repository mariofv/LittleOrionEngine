#ifndef _ANIMATIONIMPORTER_H_
#define _ANIMATIONIMPORTER_H_

#include "ResourceManagement/Resources/Animation.h"
#include "ResourceManagement/Importer/Importer.h"

#include <string>
#include <unordered_map>

struct aiAnimation;
struct aiScene;
struct aiNodeAnim;

class AnimationImporter : public Importer
{
public:

	AnimationImporter() : Importer(ResourceType::ANIMATION) {};
	~AnimationImporter() = default;

	FileData ExtractData(Path& assets_file_path) const override;
	FileData ExtractAnimationFromAssimp(const aiScene* scene, const aiAnimation* animation) const;

private:
	FileData CreateBinary(const Animation& own_format_animation) const;
	void GetCleanAnimation(const aiAnimation* animation, Animation& own_format_animation) const ;
	void TransformPositions(const aiNodeAnim* ai_node, std::unordered_map<double, float4x4>& frames) const;

};
#endif

