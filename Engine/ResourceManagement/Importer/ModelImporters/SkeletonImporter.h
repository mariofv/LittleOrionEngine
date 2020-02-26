#pragma once
struct aiScene;
struct aiNode;
#include <assimp/mesh.h>
#include <string>
#include <ResourceManagement/Resources/Skeleton.h>

class SkeletonImporter
{
public:
	SkeletonImporter() = default;
	~SkeletonImporter() = default;

	bool ImportSkeleton(const aiScene* scene, const aiMesh* mesh,std::string& output_file) const;
private:
	void ImportChildBone(const aiMesh* mesh, const aiNode * previus_node, uint32_t previous_joint_index, const aiMatrix4x4& parent_transformation, aiMatrix4x4& accumulated_local_transformation,Skeleton & skeleton) const;
	aiBone * GetNodeBone(const aiMesh* mesh, const std::string & bone_name) const;
	math::float4x4 GetTransform(const aiMatrix4x4& current_transform) const;
	bool SaveBinary(const Skeleton & skeleton, const std::string& output_file) const;

	const std::string LIBRARY_SKELETON_FOLDER{ "Library/Skeleton" };
	const float SCALE_FACTOR = 0.01f;
};

