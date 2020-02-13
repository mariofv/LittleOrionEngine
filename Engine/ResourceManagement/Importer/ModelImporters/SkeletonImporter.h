#pragma once
struct aiScene;
#include <assimp/mesh.h>
#include <string>
class SkeletonImporter
{
public:
	SkeletonImporter() = default;
	~SkeletonImporter() = default;

	bool ImportSkeleton(const aiScene* scene, const aiMesh* mesh, const aiMatrix4x4& mesh_transformation, const std::string& output_file) const;
};

