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

	bool ImportSkeleton(const aiScene* scene, const aiMesh* mesh, const aiMatrix4x4& mesh_transformation, const std::string& output_file) const;
private:
	void ImportBone(const aiMesh* mesh, const aiNode * previus_node,  Skeleton::Joint * previous_joint, const aiMatrix4x4& parent_transformation, const Skeleton & skeleton) const;
	aiBone * GetNodeBone(const aiMesh* mesh, const aiString & bone_name) const;
	ComponentTransform GetTranform(const aiMatrix4x4& current_transform) const;
};

