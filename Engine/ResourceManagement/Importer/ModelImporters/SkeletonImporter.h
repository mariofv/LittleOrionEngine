#pragma once


#include "ResourceManagement/Resources/Skeleton.h"
#include "ResourceManagement/Importer/Importer.h"

#include <assimp/mesh.h>
#include <string>

struct aiScene;
struct aiNode;

class SkeletonImporter : public Importer
{
public:
	SkeletonImporter() = default;
	~SkeletonImporter() = default;

	ImportResult ImportSkeleton(const aiScene* scene, const aiMesh* mesh, const std::string& imported_file, float unit_scale_factor, Skeleton& skeleton) const;
	static math::float4x4 GetTransform(const aiMatrix4x4& current_transform, float scale_factor = 1.0);
private:
	bool SaveBinary(const Skeleton & skeleton) const;
};

