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

	FileData ExtractData(Path& assets_file_path, const Metafile& metafile) const override;
	FileData ExtractAnimationFromAssimp(const aiScene* scene, const aiAnimation* animation, float unit_scale_factor) const;

private:
	FileData CreateBinary(const Animation& own_format_animation) const;

	void GetCleanAnimation(const aiNode* root_node, const aiAnimation* animation, Animation& own_format_animation, float scale_factor) const;
	void GetChannelTransform(const aiNodeAnim* sample, size_t animation_duration,std::map<size_t, float4x4>& sample_transform) const;

};
#endif

