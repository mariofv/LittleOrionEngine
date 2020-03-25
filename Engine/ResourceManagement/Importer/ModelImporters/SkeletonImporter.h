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

	FileData ExtractData(Path& assets_file_path) const override;
	FileData ExtractSkeletonFromAssimp(const aiScene* scene, const aiMesh* mesh) const;

private:
	FileData CreateBinary(const Skeleton & skeleton) const;

	void ImportChildBone(const aiMesh* mesh, const aiNode * previus_node, uint32_t previous_joint_index, const aiMatrix4x4& parent_transformation, aiMatrix4x4& accumulated_local_transformation,Skeleton & skeleton) const;
	aiBone * GetNodeBone(const aiMesh* mesh, const std::string & bone_name) const;
	math::float4x4 GetTransform(const aiMatrix4x4& current_transform) const;
	const float SCALE_FACTOR = 0.01f;
};

#endif