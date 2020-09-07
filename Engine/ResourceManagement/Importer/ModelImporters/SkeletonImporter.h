#ifndef _SKELETONIMPORTER_H_
#define _SKELETONIMPORTER_H_

#include "ResourceManagement/Importer/Importer.h"
#include "ResourceManagement/Resources/Skeleton.h"

#include <assimp/mesh.h>
#include <string>

struct aiScene;
struct aiNode;

class SkeletonImporter : public Importer
{
public:
	SkeletonImporter() : Importer(ResourceType::SKELETON) {};
	~SkeletonImporter() = default;

	FileData ExtractSkeletonFromAssimp(const aiScene* scene, const aiMesh* mesh, float unit_scale_factor, bool complex_skeleton) const;

private:
	FileData CreateBinary(const Skeleton & skeleton) const;
	void ImportChildBone(const aiNode * previus_node, uint32_t previous_joint_index, aiMatrix4x4 & parent_global_transformation, aiMatrix4x4 & accumulated_local_transformation, Skeleton & skeleton, float unit_scale_factor) const;
};

#endif